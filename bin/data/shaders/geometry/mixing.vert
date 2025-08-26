#version 150

// === 基础uniforms ===
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

// === 混合控制参数（Screen3专用） ===
uniform float mixRatio;              // 0.0 = 纯几何变形, 1.0 = 纯模型
uniform int enableModel;             // 是否启用模型混合
uniform int enableGeometry;          // 是否启用几何变形
uniform float modelInfluence;        // 模型影响强度
uniform float geometryInfluence;     // 几何影响强度
uniform int hasValidModel;           // 是否有有效模型

// === Screen1 模型参数 ===
uniform mat4 screen1ModelMatrix;     // Screen1的模型变换矩阵

// === Screen2 几何变形参数 ===
// 流场中心点
uniform vec3 flowCenter1;
uniform vec3 flowCenter2;
uniform vec3 flowCenter3;

// 破碎参数
uniform float fractureAmount;
uniform float fractureScale;
uniform float explosionRadius;
uniform float rotationIntensity;
uniform float separationForce;

// 消散参数
uniform float dissipationAmount;
uniform float dissipationScale;
uniform float dissipationSpeed;
uniform float cloudThreshold;
uniform float edgeSoftness;

// === 输入属性 ===
in vec4 position;
in vec3 normal;
in vec4 color;

// === 输出到fragment shader ===
out vec3 worldPos;
out vec3 worldNormal;
out vec3 vertexColor;
out vec3 lightDir;
out vec3 viewDir;
out float mixValue;  // 当前顶点的实际混合值

// === 噪声函数 ===
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

// === 旋转矩阵 ===
mat3 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
}

// === Screen2几何变形计算（完全复制Screen2的逻辑） ===
vec3 calculateScreen2Deformation(vec3 originalPos, float t) {
    vec3 deformedPos = originalPos;
    
    // 1. Perlin Noise 扰动
    float timeOffset = t * 0.3;
    float noiseX = noise4d(originalPos * noiseScale, timeOffset) * 2.0 - 1.0;
    float noiseY = noise4d(originalPos * noiseScale + vec3(100.0), timeOffset) * 2.0 - 1.0;
    float noiseZ = noise4d(originalPos * noiseScale + vec3(200.0), timeOffset) * 2.0 - 1.0;
    deformedPos += vec3(noiseX, noiseY, noiseZ) * noiseStrength;
    
    // 2. 呼吸效果
    float breathPhase = sin(t * breathSpeed) * 0.5 + 0.5;
    if (length(originalPos) > 0.001) {
        vec3 breathOffset = normalize(originalPos) * breathAmount * breathPhase;
        deformedPos += breathOffset;
    }
    
    // 3. 流场效果
    float dynamicScale = mix(0.005, 0.002, clamp(flowFieldStrength / 500.0, 0.0, 1.0));
    float flowTime = t * 0.15;
    
    float n1 = noise4d(vec3(deformedPos.y * dynamicScale, deformedPos.z * dynamicScale, 0.0), flowTime);
    float n2 = noise4d(vec3(deformedPos.z * dynamicScale, deformedPos.x * dynamicScale, 100.0), flowTime);
    float n3 = noise4d(vec3(deformedPos.x * dynamicScale, deformedPos.y * dynamicScale, 200.0), flowTime);
    
    vec3 flow;
    float smoothFactor = mix(2.0, 4.0, clamp(flowFieldStrength / 200.0, 0.0, 1.0));
    flow.x = sin(n1 * 6.28318) * cos(deformedPos.y * 0.002) * smoothFactor;
    flow.y = sin(n2 * 6.28318) * cos(deformedPos.z * 0.002) * smoothFactor;
    flow.z = sin(n3 * 6.28318) * cos(deformedPos.x * 0.002) * smoothFactor;
    
    // 流场中心影响
    vec3 flowCenters[3] = vec3[](flowCenter1, flowCenter2, flowCenter3);
    float influenceRadius = mix(300.0, 600.0, clamp(flowFieldStrength / 300.0, 0.0, 1.0));
    
    for (int i = 0; i < 3; i++) {
        vec3 center = flowCenters[i];
        float dist = length(deformedPos - center);
        
        if (dist < influenceRadius) {
            float strength = 1.0 - (dist / influenceRadius);
            strength = smoothstep(0.0, 1.0, strength);
            
            vec3 toCenter = center - deformedPos;
            vec3 tangent = cross(toCenter, vec3(0, 1, 0));
            if (length(tangent) > 0.001) {
                tangent = normalize(tangent);
            }
            
            float spiralIntensity = mix(1.5, 3.0, clamp(flowFieldStrength / 250.0, 0.0, 1.0));
            vec3 spiral = tangent * strength * spiralIntensity + vec3(0, strength * spiralIntensity * 1.5, 0);
            flow += spiral * 1.5;
        }
    }
    
    deformedPos += flow * flowFieldStrength;
    
    // 4. 破碎效果（如果启用）
    if (fractureAmount > 0.0) {
        vec3 fractureCenter = vec3(0.0);
        vec3 toCenter = originalPos - fractureCenter;
        float distanceFromCenter = length(toCenter);
        float radialFactor = smoothstep(0.0, explosionRadius, distanceFromCenter);
        
        vec3 fractureDirection = normalize(vec3(
            noise3d(originalPos * fractureScale + vec3(100.0, 0.0, 0.0)) - 0.5,
            noise3d(originalPos * fractureScale + vec3(0.0, 100.0, 0.0)) - 0.5,
            noise3d(originalPos * fractureScale + vec3(0.0, 0.0, 100.0)) - 0.5
        ));
        
        if (length(toCenter) > 0.001) {
            vec3 radialForce = normalize(toCenter) * radialFactor;
            vec3 combinedDirection = mix(fractureDirection, radialForce, 0.6);
            
            float timeProgress = smoothstep(0.0, 1.0, t * 0.1);
            float separationDistance = timeProgress * separationForce * radialFactor;
            
            vec3 fractureOffset = combinedDirection * separationDistance * fractureAmount;
            deformedPos += fractureOffset;
        }
        
        // 旋转效果
        if (rotationIntensity > 0.0) {
            vec3 rotationAxis = normalize(vec3(
                noise3d(originalPos * fractureScale + vec3(200.0)),
                noise3d(originalPos * fractureScale + vec3(300.0)),
                noise3d(originalPos * fractureScale + vec3(400.0))
            ));
            
            float rotationSpeed = hash3d(originalPos * fractureScale) * 2.0 + 1.0;
            float rotationAngle = t * rotationSpeed * rotationIntensity;
            
            vec3 relativePos = deformedPos - originalPos;
            mat3 rotation = rotationMatrix(rotationAxis, rotationAngle);
            vec3 rotatedPos = rotation * relativePos;
            deformedPos = rotatedPos + originalPos;
        }
    }
    
    // 5. 限制偏移量
    vec3 offset = deformedPos - originalPos;
    float baseMaxOffset = 100.0;
    float flowFieldBonus = clamp(flowFieldStrength * 0.8, 0.0, 400.0);
    float fractureBonus = fractureAmount * 200.0;
    float dynamicMaxOffset = baseMaxOffset + flowFieldBonus + fractureBonus;
    
    float offsetLength = length(offset);
    if (offsetLength > dynamicMaxOffset) {
        float limitFactor = dynamicMaxOffset / offsetLength;
        limitFactor = smoothstep(0.8, 1.0, limitFactor);
        offset *= limitFactor;
        deformedPos = originalPos + offset;
    }
    
    return deformedPos;
}

// === Screen1模型位置计算（应用变换） ===
vec3 calculateScreen1Position(vec3 originalPos) {
    if (hasValidModel == 0) {
        return originalPos;
    }
    
    // 应用Screen1的模型变换矩阵
    vec4 transformedPos = screen1ModelMatrix * vec4(originalPos, 1.0);
    
    // 根据模型影响强度混合
    return mix(originalPos, transformedPos.xyz, modelInfluence);
}

// === 计算动态混合值 ===
float calculateDynamicMixValue(vec3 originalPos, float baseMixRatio) {
    // 基于位置的动态混合效果
    float positionFactor = (sin(originalPos.x * 0.01 + time * 0.2) + 1.0) * 0.5;
    float timeFactor = (sin(time * 0.1) + 1.0) * 0.5;
    
    // 组合基础混合比例和动态因子
    float dynamicMix = baseMixRatio + (positionFactor * timeFactor - 0.5) * 0.1;
    return clamp(dynamicMix, 0.0, 1.0);
}

// === 主函数 ===
void main() {
    vec3 originalPos = position.xyz;
    vec3 finalPos = originalPos;
    vec3 finalNormal = normal;
    
    // 计算Screen2几何变形位置（始终基于CubeMesh）
    vec3 geometryPos = originalPos;
    if (enableGeometry == 1) {
        geometryPos = calculateScreen2Deformation(originalPos, time);
        geometryPos = mix(originalPos, geometryPos, geometryInfluence);
    }
    
    // 计算Screen1模型位置（将CubeMesh顶点变换到模型空间）
    vec3 modelPos = originalPos;
    if (enableModel == 1 && hasValidModel == 1) {
        modelPos = calculateScreen1Position(originalPos);
    }
    
    // 计算动态混合值
    mixValue = calculateDynamicMixValue(originalPos, mixRatio);
    
    // 在几何变形和模型之间插值 - 这是关键的GPU融合
    finalPos = mix(geometryPos, modelPos, mixValue);
    
    // 计算混合后的法向量
    vec3 deformationDirection = normalize(finalPos - originalPos + vec3(0.001));
    finalNormal = mix(normal, deformationDirection, 0.3);
    
    // 传递到fragment shader
    worldPos = (modelViewMatrix * vec4(finalPos, 1.0)).xyz;
    worldNormal = normalize((normalMatrix * vec4(finalNormal, 0.0)).xyz);
    vertexColor = color.rgb;
    
    // 计算光照方向
    lightDir = normalize(lightPosition - worldPos);
    viewDir = normalize(cameraPosition - worldPos);
    
    gl_Position = modelViewProjectionMatrix * vec4(finalPos, 1.0);
}