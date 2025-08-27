#version 150

uniform mat4 modelMatrix;
uniform mat4 modelViewMatrix; 
uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec3 normal;

out vec3 worldPosition;
out vec3 worldNormal;

void main() {
    vec4 worldPos4 = modelMatrix * position;
    worldPosition = worldPos4.xyz;
    worldNormal = normalize((modelMatrix * vec4(normal, 0.0)).xyz);
    gl_Position = modelViewProjectionMatrix * position;
}