#version 150

uniform float time;
uniform float noiseScale;
uniform float noiseStrength;
uniform float breathSpeed;
uniform float breathAmount;
uniform float flowFieldStrength;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

// 光照参数
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

// 流场中心点
uniform vec3 flowCenter1;
uniform vec3 flowCenter2;
uniform vec3 flowCenter3;

// 破碎和消散参数
uniform float fractureAmount;       // 破碎强度 0.0-1.0
uniform float fractureScale;        // 破碎噪声缩放
uniform float explosionRadius;      // 爆炸半径
uniform float rotationIntensity;    // 碎片旋转强度
uniform float separationForce;      // 分离力度

in vec4 position;
in vec3 normal;
in vec4 color;

// 传递到fragment shader的变量
out vec3 worldPos;
out vec3 worldNormal;
out vec3 vertexColor;
out vec3 lightDir;
out vec3 viewDir;

// ==== 噪声函数 ====

float hash3d(vec3 p) {
    return fract(sin(dot(p, vec3(127.1, 311.7, 74.7))) * 43758.5453);
}

float noise3d(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    
    vec3 u = f * f * (3.0 - 2.0 * f);
    
    float n000 = hash3d(i + vec3(0,0,0));
    float n100 = hash3d(i + vec3(1,0,0));
    float n010 = hash3d(i + vec3(0,1,0));
    float n110 = hash3d(i + vec3(1,1,0));
    float n001 = hash3d(i + vec3(0,0,1));
    float n101 = hash3d(i + vec3(1,0,1));
    float n011 = hash3d(i + vec3(0,1,1));
    float n111 = hash3d(i + vec3(1,1,1));
    
    return mix(mix(mix(n000, n100, u.x),
                   mix(n010, n110, u.x), u.y),
               mix(mix(n001, n101, u.x),
                   mix(n011, n111, u.x), u.y), u.z);
}

float noise4d(vec3 p, float t) {
    return noise3d(p + vec3(t * 0.1, t * 0.15, t * 0.12));
}

// ==== 旋转矩阵函数 ====

mat3 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
}

// ==== 破碎效果计算 ====

vec3 calculateFractureOffset(vec3 originalPos, float t) {
    // 基于位置创建不同的破碎区域
    vec3 fractureCenter = vec3(0.0, 0.0, 0.0); // 破碎中心
    vec3 toCenter = originalPos - fractureCenter;
    float distanceFromCenter = length(toCenter);
    
    // 根据距离中心的远近计算破碎强度
    float radialFactor = smoothstep(0.0, explosionRadius, distanceFromCenter);
    
    // 创建不规则的破碎分区 - 让相邻顶点有不同的运动
    float fractureSeed = hash3d(floor(originalPos * fractureScale));
    
    // 每个分区有独特的运动方向
    vec3 fractureDirection = normalize(vec3(
        noise3d(originalPos * fractureScale + vec3(100.0, 0.0, 0.0)) - 0.5,
        noise3d(originalPos * fractureScale + vec3(0.0, 100.0, 0.0)) - 0.5,
        noise3d(originalPos * fractureScale + vec3(0.0, 0.0, 100.0)) - 0.5
    ));
    
    // 添加径向爆炸力
    vec3 radialForce = normalize(toCenter) * radialFactor;
    
    // 组合破碎方向和径向力
    vec3 combinedDirection = mix(fractureDirection, radialForce, 0.6);
    
    // 时间控制的分离距离
    float timeProgress = smoothstep(0.0, 1.0, t * 0.1); // 缓慢的时间进程
    float separationDistance = timeProgress * separationForce * radialFactor;
    
    // 添加随机的抖动
    vec3 jitter = vec3(
        noise4d(originalPos * fractureScale * 2.0, t) - 0.5,
        noise4d(originalPos * fractureScale * 2.0 + vec3(50.0), t) - 0.5,
        noise4d(originalPos * fractureScale * 2.0 + vec3(100.0), t) - 0.5
    ) * 5.0;
    
    return combinedDirection * separationDistance + jitter;
}

// ==== 碎片旋转计算 ====

vec3 applyfractureRotation(vec3 pos, vec3 originalPos, float t) {
    // 基于原始位置创建独特的旋转轴和速度
    vec3 rotationAxis = normalize(vec3(
        noise3d(originalPos * fractureScale + vec3(200.0)),
        noise3d(originalPos * fractureScale + vec3(300.0)),
        noise3d(originalPos * fractureScale + vec3(400.0))
    ));
    
    // 旋转速度基于位置
    float rotationSpeed = hash3d(originalPos * fractureScale) * 2.0 + 1.0;
    float rotationAngle = t * rotationSpeed * rotationIntensity;
    
    // 计算相对于碎片中心的位置
    vec3 fractureCenter = originalPos; // 每个顶点围绕其原始位置旋转
    vec3 relativePos = pos - fractureCenter;
    
    // 应用旋转
    mat3 rotation = rotationMatrix(rotationAxis, rotationAngle);
    vec3 rotatedPos = rotation * relativePos;
    
    return rotatedPos + fractureCenter;
}

// ==== 流场计算（保持原有功能）====

vec3 calculateFlowField(vec3 pos, float t) {
    float scale = 0.008;
    float flowTime = t * 0.15;
    
    float n1 = noise4d(vec3(pos.y * scale, pos.z * scale, 0.0), flowTime);
    float n2 = noise4d(vec3(pos.z * scale, pos.x * scale, 100.0), flowTime);
    float n3 = noise4d(vec3(pos.x * scale, pos.y * scale, 200.0), flowTime);
    
    vec3 flow;
    flow.x = sin(n1 * 6.28318) * cos(pos.y * 0.005);
    flow.y = sin(n2 * 6.28318) * cos(pos.z * 0.005);
    flow.z = sin(n3 * 6.28318) * cos(pos.x * 0.005);
    
    vec3 flowCenters[3];
    flowCenters[0] = flowCenter1;
    flowCenters[1] = flowCenter2; 
    flowCenters[2] = flowCenter3;
    
    for (int i = 0; i < 3; i++) {
        vec3 center = flowCenters[i];
        float dist = length(pos - center);
        
        if (dist < 150.0) {
            float strength = 1.0 - (dist / 150.0);
            vec3 toCenter = center - pos;
            vec3 tangent = cross(toCenter, vec3(0, 1, 0));
            if (length(tangent) > 0.001) {
                tangent = normalize(tangent);
            }
            vec3 spiral = tangent * strength + vec3(0, strength * 2.0, 0);
            flow += spiral * 0.5;
        }
    }
    
    flow += vec3(
        sin(t * 0.3 + pos.x * 0.01) * 0.2,
        cos(t * 0.2) * 0.3,
        sin(t * 0.25 + pos.z * 0.01) * 0.2
    );
    
    return flow;
}

// ==== 法向量重计算 ====

vec3 calculateDeformedNormal(vec3 originalPos, vec3 deformedPos, vec3 originalNormal) {
    // 对于破碎效果，我们需要重新计算法向量
    // 简化方法：基于破碎方向调整法向量
    vec3 fractureDirection = calculateFractureOffset(originalPos, time);
    
    if (length(fractureDirection) > 0.001) {
        // 让法向量朝向破碎方向倾斜
        vec3 newNormal = mix(originalNormal, normalize(fractureDirection), fractureAmount * 0.3);
        return normalize(newNormal);
    }
    
    return originalNormal;
}

// ==== 主函数 ====

void main() {
    vec3 originalPos = position.xyz;
    vec3 newPos = originalPos;
    
    // 1. 原有的Perlin Noise随机扰动
    float timeOffset = time * 0.3;
    
    float noiseX = noise4d(originalPos * noiseScale, timeOffset) * 2.0 - 1.0;
    float noiseY = noise4d(originalPos * noiseScale + vec3(100.0), timeOffset) * 2.0 - 1.0;
    float noiseZ = noise4d(originalPos * noiseScale + vec3(200.0), timeOffset) * 2.0 - 1.0;
    
    newPos += vec3(noiseX, noiseY, noiseZ) * noiseStrength;
    
    // 2. 呼吸效果
    float breathPhase = sin(time * breathSpeed) * 0.5 + 0.5;
    vec3 breathOffset = normalize(originalPos) * breathAmount * breathPhase;
    newPos += breathOffset;
    
    // 3. 流场效果
    vec3 flowForce = calculateFlowField(originalPos, time);
    newPos += flowForce * flowFieldStrength;
    
    // 4. === 新增：破碎效果 ===
    if (fractureAmount > 0.0) {
        // 计算破碎偏移
        vec3 fractureOffset = calculateFractureOffset(originalPos, time);
        newPos += fractureOffset * fractureAmount;
        
        // 应用碎片旋转
        newPos = applyfractureRotation(newPos, originalPos, time);
    }
    
    // 5. 限制偏移（根据破碎程度调整限制）
    vec3 offset = newPos - originalPos;
    float maxOffset = 100.0 + fractureAmount * 200.0; // 破碎时允许更大偏移
    if (length(offset) > maxOffset) {
        offset = normalize(offset) * maxOffset;
        newPos = originalPos + offset;
    }
    
    // 计算变形后的法向量
    vec3 deformedNormal = calculateDeformedNormal(originalPos, newPos, normal);
    
    // 传递到fragment shader的变量
    worldPos = (modelViewMatrix * vec4(newPos, 1.0)).xyz;
    worldNormal = normalize((normalMatrix * vec4(deformedNormal, 0.0)).xyz);
    vertexColor = color.rgb;
    
    // 计算光照方向和视线方向
    lightDir = normalize(lightPosition - worldPos);
    viewDir = normalize(cameraPosition - worldPos);
    
    gl_Position = modelViewProjectionMatrix * vec4(newPos, 1.0);
}