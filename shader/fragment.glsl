#version 330 core

in vec2 UV;
in vec4 pos;
in vec4 normal;
in vec4 shadow_coord;
in vec4 shadow_coord2;

layout (location = 0) out vec4 fColor;

// uniform mat4 MVP;

// uniform sampler2D TextureSampler;
uniform sampler2D depth_texture;
uniform sampler2D depth_texture2;
uniform sampler2D depth_texture_trans;
uniform sampler2D depth_texture_trans2;
uniform vec3 Eye;
uniform vec3 Light;
uniform vec3 Light2;
uniform vec4 flag;

void main() {
    // fColor = vec4(1.0, 0.5, 0.25, 1.0);
    // MVP;
    vec3 pos3;
    pos3.xyz = pos.xyz;
    vec3 view_direct = Eye - pos3;
    vec3 Light_vec = Light - pos3;
    vec3 Light_vec2 = Light2 - pos3;
    vec3 normal3;
    normal3.xyz = normal.xyz;
    float strength = 90.0;
    float strength2 = 135.0;
    float env = 0.1;
    float kd = 0.3;
    float kr = 1.0;

    float d2 = dot(Light_vec, Light_vec);
    float d22 = dot(Light_vec2, Light_vec2);
    view_direct = normalize(view_direct);
    Light_vec = normalize(Light_vec);
    Light_vec2 = normalize(Light_vec2);
    normal3 = normalize(normal3);

    vec3 out_vec = 2.0 * dot(Light_vec, normal3) * normal3 - Light_vec;
    vec3 out_vec2 = 2.0 * dot(Light_vec2, normal3) * normal3 - Light_vec2;

    float f = 1.0;
    float f2 = 1.0;

    vec4 sc_post_w = shadow_coord / shadow_coord.w;
    vec4 sc_post_w2 = shadow_coord2 / shadow_coord2.w;

    float bias = 0.00001 * tan(acos(dot(normal3, Light_vec)));
    float bias2 = 0.00001 * tan(acos(dot(normal3, Light_vec2)));

    bias = clamp(bias, 0.00001, 0.01);
    bias2 = clamp(bias2, 0.00001, 0.01);
    if (texture(depth_texture, sc_post_w.xy).x + bias < sc_post_w.z) {
        f = 0.0;
    }
    if (f > 0.0 && texture(depth_texture_trans, sc_post_w.xy).x + bias < sc_post_w.z) {
        f = f * 0.8;
    }
    if (texture(depth_texture2, sc_post_w2.xy).x + bias2 < sc_post_w2.z) {
        f2 = 0.0;
    }
    if (f2 > 0.0 && texture(depth_texture_trans2, sc_post_w2.xy).x + bias2 < sc_post_w2.z) {
        f2 = f2 * 0.8;
    }
    // f = f2 = 0.5;

    float I = 0.0;
    I += env;
    if (flag.x < 0. && dot(Light_vec, normal3) < 0.) {
        normal3 = -normal3;
    }
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
    if (flag.x < 0. && dot(Light_vec2, normal3) < 0.) {
        normal3 = -normal3;
    }
    if (dot(Light_vec2, normal3) > 0.) {
        float I_diffuse = strength2 * kd / d22 * dot(Light_vec2, normal3);
        if (I_diffuse > 0.) {
            I += f2 * I_diffuse;
        }
        float I_reflect = strength2 * kr / d22 * dot(out_vec2, view_direct) * dot(out_vec2, view_direct);
        if (I_reflect > 0.) {
            I += f2 * I_reflect;
        }
    }

    // I = 0.5;
    fColor = vec4(I, I, I, 0.2);
    
    // fColor.rgb = texture(TextureSampler, UV).rgb;
}