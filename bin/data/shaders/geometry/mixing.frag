#version 150

// === 光照参数 ===
uniform vec3 lightColor;
uniform vec3 ambientColor;
uniform float shininess;
uniform float lightIntensity;

// === 混合控制参数 ===
uniform float mixRatio;
uniform int enableModel;
uniform int enableGeometry;
uniform float modelInfluence;
uniform float geometryInfluence;
uniform int hasValidModel;

// === 消散效果参数 ===
uniform float time;
uniform float dissipationAmount;
uniform float dissipationScale;
uniform float dissipationSpeed;
uniform float cloudThreshold;
uniform float edgeSoftness;

// === 从vertex shader传来的变量 ===
in vec3 worldPos;
in vec3 worldNormal;
in vec3 vertexColor;
in vec3 lightDir;
in vec3 viewDir;
in float mixValue;  // 当前像素的混合值

out vec4 outputColor;

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

// === 分形噪声 ===
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

// === 消散计算 ===
float calculateDissipation(vec3 pos, float t) {
    if (dissipationAmount < 0.01) {
        return 1.0; // 不消散
    }
    
    // 多层噪声创造复杂的消散图案
    float mainPattern = fractalNoise(pos * dissipationScale + vec3(t * dissipationSpeed * 0.1), 4);
    float detailPattern = fractalNoise(pos * dissipationScale * 3.0 + vec3(t * dissipationSpeed * 0.3), 3);
    float edgeNoise = noise3d(pos * dissipationScale * 6.0 + vec3(t * dissipationSpeed * 0.5));
    
    float combined = mainPattern * 0.6 + detailPattern * 0.3 + edgeNoise * 0.1;
    
    // 基于混合值调整消散阈值
    float dynamicThreshold = mix(cloudThreshold, cloudThreshold * 0.7, mixValue);
    float dissipation = smoothstep(dynamicThreshold - edgeSoftness, dynamicThreshold + edgeSoftness, combined);
    
    return mix(1.0, dissipation, dissipationAmount);
}

// === 混合状态颜色 ===
vec3 getMixStateColor(float mixVal) {
    // 根据混合状态返回不同颜色
    vec3 geometryColor = vec3(0.3, 0.8, 1.0);  // 蓝色 - 几何变形
    vec3 modelColor = vec3(1.0, 0.6, 0.2);     // 橙色 - 模型
    vec3 mixedColor = vec3(0.8, 0.8, 0.8);     // 灰白色 - 混合状态
    
    if (mixVal < 0.1) {
        return geometryColor;
    } else if (mixVal > 0.9) {
        return modelColor;
    } else {
        // 混合区域使用渐变色
        vec3 color1 = mix(geometryColor, mixedColor, smoothstep(0.1, 0.5, mixVal));
        vec3 color2 = mix(mixedColor, modelColor, smoothstep(0.5, 0.9, mixVal));
        return mix(color1, color2, smoothstep(0.1, 0.9, mixVal));
    }
}

// === 边缘发光效果 ===
float calculateRimGlow(vec3 normal, vec3 viewDirection, float intensity) {
    float rim = 1.0 - max(dot(normal, viewDirection), 0.0);
    return pow(rim, 2.0) * intensity;
}

// === 混合可视化效果 ===
vec3 addMixVisualization(vec3 baseColor, float mixVal) {
    // 在混合边界添加发光效果
    float edgeGlow = 0.0;
    
    // 检测混合边界
    float gradientStrength = abs(mixVal - 0.5) * 2.0; // 0在中心，1在边缘
    if (gradientStrength < 0.8) {
        edgeGlow = (0.8 - gradientStrength) / 0.8;
        edgeGlow = pow(edgeGlow, 2.0);
    }
    
    // 添加时间变化的混合指示
    float pulseEffect = (sin(time * 2.0) + 1.0) * 0.5 * 0.3;
    vec3 mixIndicator = getMixStateColor(mixVal) * (edgeGlow * 0.4 + pulseEffect * edgeGlow);
    
    return baseColor + mixIndicator;
}

// === 主函数 ===
void main() {
    // 计算消散
    float dissipation = calculateDissipation(worldPos, time);
    
    // 消散像素丢弃
    if (dissipation < 0.5) {
        discard;
    }
    
    // === 基础颜色计算 ===
    vec3 baseColor = getMixStateColor(mixValue);
    
    // 根据启用状态调整颜色
    if (enableModel == 0 && enableGeometry == 0) {
        baseColor = vec3(0.5, 0.5, 0.5); // 灰色 - 无效果
    } else if (enableModel == 0) {
        baseColor = vec3(0.3, 0.8, 1.0); // 纯几何色
    } else if (enableGeometry == 0) {
        baseColor = vec3(1.0, 0.6, 0.2); // 纯模型色
    }
    
    // === 光照计算 ===
    vec3 normal = normalize(worldNormal);
    vec3 light = normalize(lightDir);
    vec3 view = normalize(viewDir);
    
    // 环境光
    vec3 ambient = ambientColor * baseColor;
    
    // 漫反射
    float NdotL = max(dot(normal, light), 0.0);
    vec3 diffuse = lightColor * baseColor * NdotL * lightIntensity;
    
    // 镜面反射
    vec3 reflectDir = reflect(-light, normal);
    float RdotV = max(dot(reflectDir, view), 0.0);
    float specular = pow(RdotV, shininess);
    vec3 specularColor = lightColor * specular * lightIntensity * 0.3;
    
    // 边缘发光 - 基于混合状态调整强度
    float rimIntensity = mix(0.3, 0.8, abs(mixValue - 0.5) * 2.0);
    float rimGlow = calculateRimGlow(normal, view, rimIntensity);
    vec3 rimColor = getMixStateColor(mixValue) * rimGlow * 0.6;
    
    // === 消散边缘效果 ===
    vec3 edgeEffect = vec3(0.0);
    if (dissipationAmount > 0.01) {
        float edgeDistance = abs(dissipation - 0.5) * 2.0;
        vec3 edgeColorTint = vec3(1.2, 0.9, 0.6);
        float edgeIntensity = 1.0 - smoothstep(0.0, 0.3, edgeDistance);
        edgeEffect = edgeColorTint * edgeIntensity * 0.5;
    }
    
    // === 最终颜色组合 ===
    vec3 finalColor = ambient + diffuse + specularColor + rimColor + edgeEffect;
    
    // 添加混合可视化效果
    finalColor = addMixVisualization(finalColor, mixValue);
    
    // 根据消散程度调整透明度
    float alpha = smoothstep(0.5, 1.0, dissipation);
    
    // 基于混合状态调整透明度
    if (hasValidModel == 0 && enableModel == 1) {
        alpha *= 0.7; // 没有模型时降低透明度
    }
    
    // 色调映射和gamma校正
    finalColor = finalColor / (finalColor + vec3(1.0));
    finalColor = pow(finalColor, vec3(1.0/2.2));
    
    outputColor = vec4(finalColor, alpha);
}