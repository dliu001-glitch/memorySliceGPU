#version 150

in vec3 worldPosition;
in vec3 worldNormal;

out vec4 outputColor;

void main() {
    outputColor = vec4(worldPosition.x, worldPosition.y, worldPosition.z, 1.0);
}