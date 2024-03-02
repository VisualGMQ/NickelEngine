#version 450

layout(location = 0) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform ColorUniform {
    vec4 color;
} colorUniform;

layout(binding = 2) uniform sampler2D mySampler;

void main() {
    outColor = texture(mySampler, fragUV) * colorUniform.color;
}