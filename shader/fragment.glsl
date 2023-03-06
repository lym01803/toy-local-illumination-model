#version 330 core

in vec2 UV;

layout (location = 0) out vec4 fColor;

// uniform mat4 MVP;

uniform sampler2D TextureSampler;

void main() {
    // fColor = vec4(1.0, 0.5, 0.25, 1.0);
    // MVP;
    // fColor = fragmentColor;
    fColor.rgb = texture(TextureSampler, UV).rgb;
}