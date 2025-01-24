#version 450

layout(location = 0) in vec3 texCoord;
layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform samplerCube combinedSampler;

void main() {
    outColor = texture(combinedSampler, texCoord);
}