#version 330 core

in vec2 UV;
in vec4 pos;
in vec4 normal;
in vec4 shadow_coord;

layout (location = 0) out vec4 fColor;

// uniform mat4 MVP;

// uniform sampler2D TextureSampler;
uniform sampler2D depth_texture;
uniform vec3 Eye;
uniform vec3 Light;
uniform mat4 shadow_matrix1;

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
    float kd = 0.5;
    float kr = 1.0;
    float d2 = dot(Light_vec, Light_vec);
    view_direct = normalize(view_direct);
    Light_vec = normalize(Light_vec);
    normal3 = normalize(normal3);

    vec3 out_vec = 2.0 * dot(Light_vec, normal3) * normal3 - Light_vec;

    float f = 1.0;

    vec4 sc_post_w = shadow_coord / shadow_coord.w;

    float bias = 0.00001 * tan(acos(dot(normal3, Light_vec)));
    bias = clamp(bias, 0.0, 0.001);
    if (texture(depth_texture, sc_post_w.xy).x + bias < sc_post_w.z) {
        f = 0.0;
    }

    float I = 0.0;
    I += env;
    if (dot(Light_vec, normal3) > 0.) {
        float I_diffuse = strength * kd / d2 * dot(Light_vec, normal3);
        if (I_diffuse > 0.) {
            I += f * I_diffuse;
        }
        float I_reflect = strength * kr / d2 * dot(out_vec, view_direct) * dot(out_vec, view_direct);
        if (I_reflect > 0.) {
            I += f * I_reflect;
        }
    }

    fColor = vec4(I, I, I, 1.0);
    
    // fColor.rgb = texture(TextureSampler, UV).rgb;
}