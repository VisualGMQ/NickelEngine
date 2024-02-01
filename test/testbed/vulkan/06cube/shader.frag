#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, texcoord);
}