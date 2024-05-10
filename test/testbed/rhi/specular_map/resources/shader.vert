#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out VS_OUT {
    vec2 fragUV;
    vec3 normal;
    vec3 fragPos;
} vs_out;

layout(binding = 0) uniform MyUniform {
    mat4 model;
    mat4 view;
    mat4 proj;
} MVP;

void main() {
    gl_Position = MVP.proj * MVP.view * MVP.model * vec4(inPosition, 1.0);

    mat3 normalMat = transpose(inverse(mat3(MVP.model)));

    vs_out.fragUV = inUV;
    vs_out.normal = normalize(normalMat * inNormal);
    vs_out.fragPos = vec3(MVP.model * vec4(inPosition, 1.0));
}