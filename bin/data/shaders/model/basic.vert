
#version 150

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;

in vec4 position;
in vec3 normal;
in vec4 color;
in vec2 texcoord;

out vec3 worldPos;
out vec3 worldNormal;
out vec3 vertexColor;
out vec3 lightDir;
out vec3 viewDir;
out vec2 texCoord;

void main() {
    // 计算世界坐标系中的位置和法向量
    worldPos = (modelViewMatrix * position).xyz;
    worldNormal = normalize((normalMatrix * vec4(normal, 0.0)).xyz);
    
    // 传递顶点颜色和纹理坐标
    vertexColor = color.rgb;
    texCoord = texcoord;
    
    // 计算光照方向和视线方向
    lightDir = normalize(lightPosition - worldPos);
    viewDir = normalize(cameraPosition - worldPos);
    
    // 最终顶点位置
    gl_Position = modelViewProjectionMatrix * position;
}