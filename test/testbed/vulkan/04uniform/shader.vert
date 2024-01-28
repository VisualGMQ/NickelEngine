#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 proj;
    mat4 view;
} ubo;

layout(push_constant) uniform PushConsts {
	mat4 model;
} pushConsts;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = ubo.proj * ubo.view * pushConsts.model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}