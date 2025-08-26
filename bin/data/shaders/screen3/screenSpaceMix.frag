#version 150

uniform sampler2D screen1PositionTex;
uniform sampler2D screen1DepthTex;
uniform sampler2D screen2PositionTex;
uniform sampler2D screen2DepthTex;

uniform float mixRatio;
uniform int enableModel;
uniform int enableGeometry;
uniform float modelInfluence;
uniform float geometryInfluence;

uniform vec2 resolution;
uniform float time;

// 光照参数
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform vec3 lightColor;
uniform vec3 ambientColor;
uniform float lightIntensity;
uniform float shininess;

// 调试参数
uniform int showScreen1Debug;
uniform int showScreen2Debug;

in vec2 vTexCoord;
out vec4 outputColor;

void main() {
    vec2 uv = gl_FragCoord.xy / resolution;
    
    // 采样两个屏幕的位置数据
    vec4 pos1 = texture(screen1PositionTex, uv);
    vec4 pos2 = texture(screen2PositionTex, uv);
    
    // 检查有效性 (alpha > 0.5 表示有效像素)
    bool valid1 = (pos1.a > 0.5);
    bool valid2 = (pos2.a > 0.5);
    
    // 调试模式
    if (showScreen1Debug == 1) {
        outputColor = valid1 ? pos1 : vec4(0.2, 0.2, 0.2, 1.0);
        return;
    }
    if (showScreen2Debug == 1) {
        outputColor = valid2 ? pos2 : vec4(0.2, 0.2, 0.2, 1.0);
        return;
    }
    
    vec3 finalWorldPos;
    bool hasValidPos = false;
    
    if (valid1 && valid2 && enableModel == 1 && enableGeometry == 1) {
        // 两者都有效：根据mixRatio混合
        finalWorldPos = mix(pos2.xyz, pos1.xyz, mixRatio);
        hasValidPos = true;
    } else if (valid1 && enableModel == 1) {
        // 只有Screen1有效
        finalWorldPos = pos1.xyz;
        hasValidPos = true;
    } else if (valid2 && enableGeometry == 1) {
        // 只有Screen2有效  
        finalWorldPos = pos2.xyz;
        hasValidPos = true;
    }
    
    if (!hasValidPos) {
        // 背景色
        outputColor = vec4(0.1, 0.1, 0.1, 1.0);
        return;
    }
    
    // 简单的光照计算
    vec3 baseColor = vec3(0.8, 0.8, 0.9);
    vec3 lightDir = normalize(lightPosition - finalWorldPos);
    vec3 viewDir = normalize(cameraPosition - finalWorldPos);
    
    // 假设法向量朝上
    vec3 normal = vec3(0.0, 1.0, 0.0);
    
    vec3 ambient = ambientColor * baseColor;
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightColor * baseColor * NdotL * lightIntensity;
    
    vec3 finalColor = ambient + diffuse;
    finalColor = pow(finalColor, vec3(1.0/2.2)); // gamma校正
    
    outputColor = vec4(finalColor, 1.0);
}