#version 450 core

in vec2 TexCoord;
in vec4 Color;

uniform sampler2D image;

out vec4 FragColor;

void main() {
    FragColor = texture(image, TexCoord) * Color;
}
