#version 300 es

precision mediump float;

in vec3 FragColor;
out vec4 Color;

void main() {
    Color = vec4(FragColor, 1.0);
}