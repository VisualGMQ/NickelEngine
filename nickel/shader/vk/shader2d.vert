#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;

layout(location = 0) out VS_OUT {
    vec2 uv;
    vec4 color;
} vs_out;

layout(binding = 0) uniform UBO {
    mat4 view; 
    mat4 project;
} ubo;

layout(push_constant) uniform PushConstant {
    mat4 model;
} push_constant;

void main() {
    vs_out.uv = inUV;
    vs_out.color = inColor;
    gl_Position = ubo.project * ubo.view * push_constant.model * vec4(inPos, 1.0);
}
