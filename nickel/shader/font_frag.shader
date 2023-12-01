#version 450 core

in vec2 TexCoord;
in vec4 Color;

uniform sampler2D image;

out vec4 FragColor;

void main() {
    float r = texture(image, TexCoord).r;
    if (r == 0) {
        discard;
    }
    FragColor = Color;
}
