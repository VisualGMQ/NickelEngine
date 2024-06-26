#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;

layout (location = 0) out VS_OUT{
    vec2 fragUV;
    mat3 TBN;
} vs_out;

layout(binding = 0) uniform MyUniform {
    mat4 model;
    mat4 view;
    mat4 proj;
} MVP;

void main() {
    gl_Position = MVP.proj * MVP.view * MVP.model * vec4(inPosition, 1.0);

    mat3 normalMat = mat3(transpose(inverse(MVP.model)));

    vs_out.fragUV = inUV;

    vec3 T = normalize(normalMat * normalize(inTangent.xyz));
    vec3 N = normalize(normalMat * normalize(inNormal));
    vec3 B = normalize(cross(N, T));

    vs_out.TBN = mat3(T, B, N);
}