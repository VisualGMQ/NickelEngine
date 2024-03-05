#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 texCoord;

out vec2 fragUV;

layout(binding = 0) uniform MyUniform {
    mat4 model;
    mat4 view;
    mat4 proj;
} MVP;

layout(binding = 16) uniform PushConstant {
    mat4 model;
} pushConstant;

void main() {
    gl_Position = MVP.proj * MVP.view * MVP.model * pushConstant.model * vec4(inPosition, 1.0);
    fragUV = texCoord;
}