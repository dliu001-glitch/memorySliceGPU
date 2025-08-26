#version 150

// 光照参数
uniform vec3 lightColor;
uniform vec3 ambientColor;
uniform float shininess;
uniform float lightIntensity;

// 消散效果参数
uniform float time;
uniform float dissipationAmount;    // 消散强度 0.0-1.0
uniform float dissipationScale;     // 消散噪声缩放
uniform float dissipationSpeed;     // 消散动画速度
uniform float cloudThreshold;       // 云状效果阈值
uniform float edgeSoftness;         // 边缘柔和度

// 从vertex shader传来的变量
in vec3 worldPos;
in vec3 worldNormal;
in vec3 vertexColor;
in vec3 lightDir;
in vec3 viewDir;

out vec4 outputColor;

// ==== 3D噪声函数 ====

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

// 分形噪声 - 创造更复杂的云状图案
float fractalNoise(vec3 p, int octaves) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    
    for (int i = 0; i < octaves; i++) {
        value += amplitude * noise3d(p * frequency);
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    
    return value;
}

// 云状消散计算
float calculateDissipation(vec3 pos, float t) {
    // 多层噪声创造复杂的云状图案
    
    // 主要消散图案 - 缓慢移动的大块云状
    float mainPattern = fractalNoise(pos * dissipationScale + vec3(t * dissipationSpeed * 0.1), 4);
    
    // 细节消散 - 快速变化的细小消散点
    float detailPattern = fractalNoise(pos * dissipationScale * 3.0 + vec3(t * dissipationSpeed * 0.3), 3);
    
    // 边缘扰动 - 让消散边缘更自然
    float edgeNoise = noise3d(pos * dissipationScale * 6.0 + vec3(t * dissipationSpeed * 0.5));
    
    // 组合不同层次的图案
    float combined = mainPattern * 0.6 + detailPattern * 0.3 + edgeNoise * 0.1;
    
    // 时间驱动的消散强度变化
    float timeWave = sin(t * 0.2) * 0.5 + 0.5; // 0-1的正弦波
    //float dynamicThreshold = cloudThreshold + timeWave * 0.3;
    
    
    // 计算消散值
    //float dissipation = smoothstep(dynamicThreshold - edgeSoftness, dynamicThreshold + edgeSoftness, combined);
    
    float staticThreshold = cloudThreshold;
    float dissipation = smoothstep(staticThreshold - edgeSoftness, staticThreshold + edgeSoftness, combined);
    
    return dissipation;
}

// 边缘发光效果
float calculateRimGlow(vec3 normal, vec3 viewDirection) {
    float rim = 1.0 - max(dot(normal, viewDirection), 0.0);
    return pow(rim, 2.0);
}

void main() {
    // 声明变量
    float dissipation = 0.0;
    vec3 edgeEffect = vec3(0.0);
    float alpha = 1.0;
    
    // 检查是否需要计算消散效果
    if (dissipationAmount > 0.009) {  // 使用 0.009 而不是 0.01
        dissipation = calculateDissipation(worldPos, time);
        dissipation = mix(0.0, dissipation, dissipationAmount);
        
        // 消散像素丢弃
        if (dissipation < 0.5) {
            discard;
        }
        
        // 计算消散边缘特效
        float edgeDistance = abs(dissipation - 0.5) * 2.0;
        vec3 edgeColorTint = vec3(1.2, 0.9, 0.6);
        float edgeIntensity = 1.0 - smoothstep(0.0, 0.3, edgeDistance);
        edgeEffect = edgeColorTint * edgeIntensity * 0.5;
        
        alpha = smoothstep(0.5, 1.0, dissipation);
    }
    
    // === 计算光照 ===
    
    vec3 baseColor = vec3(1.0, 1.0, 1.0);
    vec3 normal = normalize(worldNormal);
    vec3 light = normalize(lightDir);
    vec3 view = normalize(viewDir);
    
    // 基础光照计算
    vec3 ambient = ambientColor * baseColor;
    float NdotL = max(dot(normal, light), 0.0);
    vec3 diffuse = lightColor * baseColor * NdotL * lightIntensity;
    
    // 镜面反射
    vec3 reflectDir = reflect(-light, normal);
    float RdotV = max(dot(reflectDir, view), 0.0);
    float specular = pow(RdotV, shininess);
    vec3 specularColor = lightColor * specular * lightIntensity * 0.3;
    
    // 边缘发光
    float rim = 1.0 - max(dot(normal, view), 0.0);
    float rimGlow = pow(rim, 2.0);
    vec3 rimColor = lightColor * rimGlow * 0.4;
    
    // === 最终颜色组合 ===
    vec3 finalColor = ambient + diffuse + specularColor + rimColor + edgeEffect;
    
    // 色调映射和gamma校正
    finalColor = finalColor / (finalColor + vec3(1.0));
    finalColor = pow(finalColor, vec3(1.0/2.2));
    
    outputColor = vec4(finalColor, alpha);
}