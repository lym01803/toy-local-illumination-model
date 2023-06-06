#version 330 core

layout(location = 0) out vec4 fColor;

void main() {
    fColor = vec4(gl_FragCoord.z) * 0.4;
}