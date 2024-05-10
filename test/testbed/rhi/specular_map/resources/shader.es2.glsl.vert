#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

out vec2 fragUV;
out vec3 normal;
out vec3 fragPos;

uniform MyUniform {
    mat4 model;
    mat4 view;
    mat4 proj;
} MVP;

void main() {
    gl_Position = MVP.proj * MVP.view * MVP.model * vec4(inPosition, 1.0);

    mat3 normalMat = transpose(inverse(mat3(MVP.model)));

    fragUV = inUV;
    normal = normalMat * inNormal;
    fragPos = vec3(MVP.model * vec4(inPosition, 1.0));
}