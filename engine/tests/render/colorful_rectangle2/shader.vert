#version 450

layout(location = 0) in vec2 inVertexPoint;

void main() {
    gl_Position = vec4(inVertexPoint, 0.0, 1.0);
}