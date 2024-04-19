#version 450

layout(location = 0) in FS_IN {
    vec2 uv;
    vec4 color;
} fs_in;

layout(location = 0) out vec4 FragColor;

layout(binding = 1) uniform sampler mySampler;
layout(binding = 2) uniform texture2D myTexture;

void main() {
    FragColor = texture(sampler2D(myTexture, mySampler), fs_in.uv) * fs_in.color;
}
