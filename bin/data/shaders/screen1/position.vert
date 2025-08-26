#version 150

uniform mat4 modelMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec3 normal;

// 传递世界坐标到fragment shader
out vec3 worldPosition;
out vec3 worldNormal;

void main() {
    // 计算世界坐标
    vec4 worldPos4 = modelMatrix * position;
    worldPosition = worldPos4.xyz;
    
    // 计算世界法向量
    worldNormal = normalize((modelMatrix * vec4(normal, 0.0)).xyz);
    
    // 标准投影变换
    gl_Position = modelViewProjectionMatrix * position;
}