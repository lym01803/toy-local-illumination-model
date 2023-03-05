#version 330 core

in vec4 fragmentColor;

layout (location = 0) out vec4 fColor;

// uniform mat4 MVP;

void main() {
    // fColor = vec4(1.0, 0.5, 0.25, 1.0);
    // MVP;
    fColor = fragmentColor;
}