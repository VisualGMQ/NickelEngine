#version 450

layout(location = 0) in vec2 inVertexPoint;
layout(location = 1) in vec4 inColor;
layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = vec4(inVertexPoint, 0.0, 1.0);
    fragColor = inColor;
}