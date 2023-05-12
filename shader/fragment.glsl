#version 330 core

in vec2 UV;
in vec4 pos;
in vec4 normal;

layout (location = 0) out vec4 fColor;

// uniform mat4 MVP;

uniform sampler2D TextureSampler;
uniform vec3 Eye;
uniform vec3 Light;

void main() {
    // fColor = vec4(1.0, 0.5, 0.25, 1.0);
    // MVP;
    vec3 pos3;
    pos3.xyz = pos.xyz;
    vec3 view_direct = Eye - pos3;
    vec3 Light_vec = Light - pos3;
    vec3 normal3;
    normal3.xyz = normal.xyz;
    float strength = 25.0;
    float env = 0.1;
    float kd = 0.25;
    float kr = 0.75;
    float d2 = dot(Light_vec, Light_vec);
    view_direct = normalize(view_direct);
    Light_vec = normalize(Light_vec);
    normal3 = normalize(normal3);

    vec3 out_vec = 2.0 * dot(Light_vec, normal3) * normal3 - Light_vec;

    float I = 0.0;
    I += env;
    float I_diffuse = strength * kd / d2 * dot(Light_vec, normal3);
    if (I_diffuse > 0.) {
        I += I_diffuse;
    }
    float I_reflect = strength * kr / d2 * dot(out_vec, view_direct) * dot(out_vec, view_direct);
    if (I_reflect > 0.) {
        I += I_reflect;
    }

    fColor = vec4(I, I, I, 1.0);
    
    // fColor.rgb = texture(TextureSampler, UV).rgb;
}