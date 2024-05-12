#version 430 core

in VS_OUT{
    vec2 uv;
    vec4 color;
} fs_in;

out vec4 FragColor;

layout(binding = 1) uniform sampler2D mySampler;

void main() {
    FragColor = texture(mySampler, fs_in.uv) * fs_in.color;
}
