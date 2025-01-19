#version 450

layout(location = 0) in vec3 inVertexPoint;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 texCoord;

layout(binding = 0) uniform MVP {
    mat4 proj;
    mat4 view;
    mat4 model;
} mvp;

void main() {
    gl_Position = mvp.proj * mvp.view * mvp.model * vec4(inVertexPoint, 1.0);
    texCoord = inTexCoord;
}