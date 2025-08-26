#version 150

// 从vertex shader传来的世界坐标
in vec3 worldPosition;
in vec3 worldNormal;

// 输出世界位置到纹理
out vec4 outputPosition;

void main() {
    // 将世界坐标存储到RGB通道
    // A通道设置为1.0表示这是有效的像素
    outputPosition = vec4(worldPosition.x, worldPosition.y, worldPosition.z, 1.0);
}