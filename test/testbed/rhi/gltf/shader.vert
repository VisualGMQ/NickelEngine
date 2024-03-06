#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec3 fragNormal;

layout(binding = 0) uniform MyUniform {
    mat4 model;
    mat4 view;
    mat4 proj;
} MVP;

layout(push_constant) uniform ModelMat {
    mat4 model;
} modelMat;

void main() {
    mat4 model = MVP.model * modelMat.model;
    gl_Position = MVP.proj * MVP.view * model * vec4(inPosition, 1.0);
    fragUV = texCoord;
    fragNormal = mat3(transpose(inverse(model))) * inNormal;
}