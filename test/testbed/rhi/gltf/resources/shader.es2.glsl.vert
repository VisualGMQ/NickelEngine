#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inTangent;

out VS_OUT{
    vec2 fragUV;
    vec3 inPos;
    vec3 fragPos;
    mat3 TBN;
} vs_out;

uniform MyUniform {
    mat4 view;
    mat4 proj;
} MVP;

uniform PushConstant {
    mat4 model;
} pushConstant;

void main() {
    vs_out.inPos = inPosition;

    mat4 model = pushConstant.model;
    vec4 fragPos = model * vec4(inPosition, 1.0);
    gl_Position = MVP.proj * MVP.view * fragPos;

    vs_out.fragPos = vec3(fragPos);

    mat3 normalMat = mat3(transpose(inverse(model)));

    vs_out.fragUV = inUV;

    vec3 T = normalize(normalMat * normalize(inTangent.xyz));
    vec3 N = normalize(normalMat * normalize(inNormal));
    vec3 B = normalize(cross(N, T) * inTangent.w);

    vs_out.TBN = mat3(T, B, N);
}