#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inTangent;

out vec2 fragUV;
out mat3 TBN;

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
    fragUV = inUV;

    mat3 matForTBN = mat3(transpose(inverse(model)));

    vec3 T = matForTBN * inTangent.xyz;
    vec3 N = matForTBN * inNormal;
    vec3 B = cross(N, T) * inTangent.w;

    TBN = mat3(TBN);
}