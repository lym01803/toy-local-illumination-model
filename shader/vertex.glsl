#version 330 core

layout(location = 0) in vec4 position;
// layout(location = 1) in vec4 vertexColor;
layout(location = 1) in vec2 vertexUV; 

// out vec4 fragmentColor;
out vec2 UV;

uniform mat4 MVP;

// uniform sampler2D TextureSampler;

void main() {
    gl_Position = MVP * position;
    // fragmentColor = vertexColor;
    UV = vertexUV;
}