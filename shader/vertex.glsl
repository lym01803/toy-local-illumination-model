#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 vertexColor;

out vec4 fragmentColor;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * position;
    fragmentColor = vertexColor;
}