#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec4 debugColor;

out vec4 outputColor;

void main() {
    // Simple white wireframe - no complex lighting or color mixing
    outputColor = vec4(1.0, 1.0, 1.0, 1.0);
}