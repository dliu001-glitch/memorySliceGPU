#version 120
void main() {
    // 使用屏幕坐标创建条纹图案，确保着色器在执行
    float stripe = mod(gl_FragCoord.x + gl_FragCoord.y, 20.0);
    if (stripe < 10.0) {
        gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0); // 绿色条纹
    } else {
        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); // 红色条纹
    }
}