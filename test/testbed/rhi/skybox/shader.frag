#version 450

layout(location = 0) in VS_OUT {
    vec3 fragUV;
} fs_in;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform samplerCube mySampler;

void main() {
    outColor = texture(mySampler, fs_in.fragUV);
}