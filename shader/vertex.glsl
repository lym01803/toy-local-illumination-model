#version 330 core

layout(location = 0) in vec4 position;
// layout(location = 1) in vec4 vertexColor;
layout(location = 1) in vec2 vertexUV; 
layout(location = 2) in vec4 in_normal;

// out vec4 fragmentColor;
out vec2 UV;
out vec4 pos;
out vec4 normal;
out vec4 shadow_coord;

uniform mat4 MVP;
uniform mat4 shadow_matrix1;

// uniform sampler2D TextureSampler;

void main() {
    gl_Position = MVP * position;
    pos = position;
    // fragmentColor = vertexColor;
    UV = vertexUV;
    normal = in_normal;
    shadow_coord = shadow_matrix1 * position;
    // gl_Position = shadow_coord;
}