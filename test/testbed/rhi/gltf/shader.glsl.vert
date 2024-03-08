#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inTangent;

out VS_OUT {
    vec2 fragUV;
    mat3 TBN;
} vs_out;

layout(binding = 0) uniform MyUniform {
    mat4 model;
    mat4 view;
    mat4 proj;
} MVP;

layout(binding = 16) uniform PushConstant {
    mat4 model;
} pushConstant;

void main() {
    mat4 model = MVP.model * pushConstant.model;
    gl_Position = MVP.proj * MVP.view * model * vec4(inPosition, 1.0);

    vs_out.fragUV = inUV;

    mat3 normalMat = transpose(inverse(mat3(model)));

    vec3 T = normalize(normalMat * inTangent.xyz);
    vec3 N = normalize(normalMat * inNormal);
    vec3 B = normalize(cross(N, inTangent.xyz) * inTangent.w);
    B = normalize(normalMat * B);

    vs_out.TBN = mat3(T, B, N);
}