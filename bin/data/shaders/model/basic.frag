#version 150

uniform vec3 lightColor;
uniform vec3 ambientColor;
uniform float lightIntensity;
uniform float shininess;

in vec3 worldPos;
in vec3 worldNormal;
in vec3 vertexColor;
in vec3 lightDir;
in vec3 viewDir;
in vec2 texCoord;

out vec4 outputColor;

void main() {
    // 归一化向量
    vec3 normal = normalize(worldNormal);
    vec3 light = normalize(lightDir);
    vec3 view = normalize(viewDir);
    
    // 基础材质颜色 (使用顶点颜色或默认白色)
    vec3 materialColor = length(vertexColor) > 0.1 ? vertexColor : vec3(0.8, 0.8, 0.9);
    
    // === Phong光照模型 ===
    
    // 1. 环境光
    vec3 ambient = ambientColor * materialColor;
    
    // 2. 漫反射光
    float NdotL = max(dot(normal, light), 0.0);
    vec3 diffuse = lightColor * materialColor * NdotL * lightIntensity;
    
    // 3. 镜面反射光
    vec3 reflectDir = reflect(-light, normal);
    float RdotV = max(dot(reflectDir, view), 0.0);
    float specular = pow(RdotV, shininess);
    vec3 specularColor = lightColor * specular * lightIntensity * 0.3;
    
    // 4. 边缘光效果 (Rim Lighting)
    float rim = 1.0 - max(dot(normal, view), 0.0);
    float rimEffect = pow(rim, 3.0) * 0.4;
    vec3 rimColor = lightColor * rimEffect;
    
    // === 组合所有光照 ===
    vec3 finalColor = ambient + diffuse + specularColor + rimColor;
    
    // 简单的色调映射，防止过曝
    finalColor = finalColor / (finalColor + vec3(1.0));
    
    // Gamma校正
    finalColor = pow(finalColor, vec3(1.0/2.2));
    
    // 输出最终颜色
    outputColor = vec4(finalColor, 1.0);
}