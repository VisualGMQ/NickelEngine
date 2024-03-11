#version 450

in VS_OUT {
    vec2 fragUV;
    mat3 TBN;
} fs_in;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform ColorUniform {
    vec4 color;
} colorUniform;

layout(binding = 2) uniform sampler2D mySampler;
layout(binding = 3) uniform sampler2D normalMapSampler;

const vec3 lightDir = normalize(vec3(-1, -1, -1));

void main() {
    vec3 normal = texture(normalMapSampler, fs_in.fragUV).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(fs_in.TBN * normal);
    float factor = max(dot(-lightDir, normal), 0);
    outColor = factor * vec4(texture(mySampler, fs_in.fragUV).rgb, 1.0) * colorUniform.color;
}