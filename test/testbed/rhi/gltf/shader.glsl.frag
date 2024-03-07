#version 450

in vec2 fragUV;
in mat3 TBN;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform ColorUniform {
    vec4 color;
} colorUniform;

layout(binding = 2) uniform sampler2D mySampler;
layout(binding = 3) uniform sampler2D normalMapSampler;

const vec3 lightDir = normalize(vec3(-1, -1, -1));

void main() {
    // vec3 normal = TBN * (texture(normalMapSampler, fragUV).xyz * 2 - 1);
    // float factor = max(dot(-lightDir, normal), 0);
    // outColor = factor * texture(mySampler, fragUV) * colorUniform.color;
    outColor = vec4(texture(mySampler, fragUV).rgb, 1.0) * colorUniform.color;
}