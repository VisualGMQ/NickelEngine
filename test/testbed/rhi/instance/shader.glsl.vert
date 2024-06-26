#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUV;

layout(binding = 0) uniform MyUniform {
    mat4 model;
    mat4 view;
    mat4 proj;
} MVP;

layout(binding = 1) uniform InstanceUniform {
    vec3 offset[16];
} instanceUniform;

void main() {
    gl_Position = MVP.proj * MVP.view * MVP.model * vec4(inPosition + instanceUniform.offset[1], 1.0);
    fragColor = inColor;
    fragUV = texCoord;
}