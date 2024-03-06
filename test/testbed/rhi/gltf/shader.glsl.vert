#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

out vec2 fragUV;
out vec3 fragNormal;

layout(binding = 0) uniform MyUniform {
    mat4 model;
    mat4 view;
    mat4 proj;
} MVP;

layout(binding = 16) uniform PushConstant {
    mat4 model;
} pushConstant;

void main() {
    mat4 finalModel = MVP.model * pushConstant.model;
    gl_Position = MVP.proj * MVP.view * finalModel * vec4(inPosition, 1.0);
    fragUV = inUV;
    fragNormal = mat3(transpose(inverse(finalModel))) * inNormal;
}