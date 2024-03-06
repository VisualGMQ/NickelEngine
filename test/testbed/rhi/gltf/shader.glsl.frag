#version 450

in vec2 fragUV;
in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform ColorUniform {
    vec4 color;
} colorUniform;

layout(binding = 2) uniform sampler2D mySampler;

const vec3 lightDir = normalize(vec3(-1, -1, -1));

void main() {
    float factor = max(dot(-lightDir, normalize(fragNormal)), 0);
    outColor = factor * texture(mySampler, fragUV) * colorUniform.color;
}