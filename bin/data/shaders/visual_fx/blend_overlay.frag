
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
    float dynamicThreshold = cloudThreshold + timeWave * 0.3;
    
    // 计算消散值
    float dissipation = smoothstep(dynamicThreshold - edgeSoftness, dynamicThreshold + edgeSoftness, combined);
    
    return dissipation;
}

// 边缘发光效果
float calculateRimGlow(vec3 normal, vec3 viewDirection) {
    float rim = 1.0 - max(dot(normal, viewDirection), 0.0);
    return pow(rim, 2.0);
}

void main() {
    // 计算消散值
    float dissipation = calculateDissipation(worldPos, time);
    
    // 应用消散强度控制
    dissipation = mix(0.0, dissipation, dissipationAmount);
    
    // Discard像素 - 这是关键！创造真正的消失效果
    if (dissipation < 0.5) {
        discard; // 像素完全消失，不是透明
    }
    
    // === 计算剩余像素的光照 ===
    
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
    
    // 边缘发光 - 增强消散边缘的视觉效果
    float rimGlow = calculateRimGlow(normal, view);
    vec3 rimColor = lightColor * rimGlow * 0.4;
    
    // === 消散边缘特效 ===
    
    // 消散边缘着色 - 让即将消失的部分有特殊颜色
    float edgeDistance = abs(dissipation - 0.5) * 2.0; // 0.0在边缘，1.0在中心
    vec3 edgeColorTint = vec3(1.2, 0.9, 0.6); // 温暖的边缘色调
    
    // 消散边缘强度
    float edgeIntensity = 1.0 - smoothstep(0.0, 0.3, edgeDistance);
    vec3 edgeEffect = edgeColorTint * edgeIntensity * 0.5;
    
    // === 最终颜色组合 ===
    
    vec3 finalColor = ambient + diffuse + specularColor + rimColor + edgeEffect;
    
    // 根据消散程度调整整体透明度 - 创造渐变消失效果
    float alpha = smoothstep(0.5, 1.0, dissipation);
    
    // 色调映射和gamma校正
    finalColor = finalColor / (finalColor + vec3(1.0));
    finalColor = pow(finalColor, vec3(1.0/2.2));
    
    outputColor = vec4(finalColor, alpha);
}