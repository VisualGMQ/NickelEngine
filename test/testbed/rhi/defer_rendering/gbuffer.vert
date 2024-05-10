#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;

layout(location = 0) out VsOut {
    vec3 position;
    vec2 uv;
    vec3 normal;
} vs_out;

layout(binding = 0) uniform MyUniform {
    mat4 view;
    mat4 proj;
} MVP;

layout(push_constant) uniform PushConstant {
    mat4 model;
} pushConstant;

void main() {
    vec4 tempPos = vec4(inPosition, 1.0);
    gl_Position = MVP.proj * MVP.view * pushConstant.model * tempPos;

    vs_out.position = vec3(pushConstant.model * tempPos);
    vs_out.uv = inUV;

    mat3 mNormal = transpose(inverse(mat3(pushConstant.model)));
    vs_out.normal = mNormal * normalize(inNormal);	
	// outTangent = mNormal * normalize(inTangent);
}