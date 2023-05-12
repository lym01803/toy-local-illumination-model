#version 330 core

layout(location = 0) in vec4 position;
// layout(location = 1) in vec4 vertexColor;
layout(location = 1) in vec2 vertexUV; 
layout(location = 2) in vec4 in_normal;

// out vec4 fragmentColor;
out vec2 UV;
out vec4 pos;
out vec4 normal;

uniform mat4 MVP;

// uniform sampler2D TextureSampler;

void main() {
    gl_Position = MVP * position;
    pos = position;
    // fragmentColor = vertexColor;
    UV = vertexUV;
    normal = in_normal;
}