#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

out VS_OUT{
    vec3 normal;
} vs_out;

layout(binding = 0) uniform MyUniform {
    mat4 model;
    mat4 view;
    mat4 proj;
} MVP;

void main() {
    gl_Position = MVP.proj * MVP.view * MVP.model * vec4(inPosition, 1.0);
    vs_out.normal = normalize(mat3(transpose(inverse(MVP.model))) * inNormal);
}