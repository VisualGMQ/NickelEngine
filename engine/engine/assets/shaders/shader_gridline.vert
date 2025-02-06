#version 450

layout(location = 0) in vec3 inVertexPoint;
layout(location = 1) in vec4 inColor;
layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform Project {
    mat4 proj;
} project;

layout(push_constant) uniform ModelView {
    mat4 model;
    mat4 view;
} mv;

void main() {
    gl_Position = project.proj * mv.view * mv.model * vec4(inVertexPoint, 1.0);
    fragColor = inColor;
}