#version 450 core

in vec2 TexCoord;
in vec4 Color;

uniform sampler2D image;

out vec4 FragColor;

void main() {
    float r = texture(image, TexCoord).r;
    FragColor = Color * vec4(1, 1, 1, r);
}
