#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(binding = 0) uniform MyUniform {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 lightMatrix;
} MVP;

void main() {
    gl_Position = MVP.lightMatrix * MVP.model * vec4(inPosition, 1.0);
}