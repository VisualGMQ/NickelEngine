#version 450

in VS_OUT {
    vec3 normal;
} fs_in;

out vec4 outColor;

layout(binding = 1) uniform ColorUniform {
    vec4 color;
} Color;

vec3 lightDir = normalize(vec3(-1, -1, -1));

void main() {
    // outColor = Color.color;
    float factor = max(0, dot(-lightDir, fs_in.normal));
    outColor = factor * vec4(0, 1, 1, 1);
}