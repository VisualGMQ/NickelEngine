#version 300 es

precision mediump float;

in vec2 fragUV;
in mat3 TBN;

out vec4 outColor;

uniform sampler2D mySampler;
uniform sampler2D normalMapSampler;

const vec3 lightDir = normalize(vec3(-1, -1, -1));

void main() {
    vec3 normal = texture(normalMapSampler, fragUV).rgb;
    mat3 TBN = mat3(normalize(TBN[0]), normalize(TBN[1]), normalize(TBN[2]));
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);
    float factor = max(dot(-lightDir, normal), 0.0);
    outColor = factor * vec4(texture(mySampler, fragUV).rgb, 1.0);
}