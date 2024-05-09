#version 450 core

in VS_OUT {
    vec2 fragUV;
    mat3 TBN;
} fs_in;

out vec4 outColor;

layout(binding = 1) uniform sampler2D mySampler;
layout(binding = 2) uniform sampler2D normalMapSampler;

const vec3 lightDir = normalize(vec3(-1, -1, -1));

void main() {
    vec3 normal = texture(normalMapSampler, fs_in.fragUV).rgb;
    mat3 TBN = mat3(normalize(fs_in.TBN[0]), normalize(fs_in.TBN[1]), normalize(fs_in.TBN[2]));
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);
    float factor = max(dot(-lightDir, normal), 0);
    outColor = factor * vec4(texture(mySampler, fs_in.fragUV).rgb, 1.0);
}