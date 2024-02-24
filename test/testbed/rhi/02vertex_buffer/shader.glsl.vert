#version 430 core

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

out vec3 FragColor;

void main() {
    gl_Position = vec4(inPosition, 0, 1.0);
    FragColor = inColor;
}
