#version 430 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;

out VS_OUT {
    vec2 uv;
    vec4 color;
} vs_out;

layout(binding = 16) uniform PushConstant {
    mat4 project;
    mat4 view;
} push_constant;

void main() {
    vs_out.uv = inUV;
    vs_out.color = inColor;
    gl_Position = push_constant.project  * push_constant.view * vec4(inPos, 1.0);
}
