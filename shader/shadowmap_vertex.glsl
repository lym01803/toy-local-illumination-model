#version 330 core

uniform mat4 MVP;
layout (location = 0) in vec4 position;

void main() {
    gl_Position = MVP * position;
}