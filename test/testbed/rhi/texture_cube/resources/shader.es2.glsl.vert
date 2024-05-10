#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 texCoord;

out vec3 fragColor;
out vec2 fragUV;

uniform MyUniform {
    mat4 model;
    mat4 view;
    mat4 proj;
} MVP;

void main() {
    gl_Position = MVP.proj * MVP.view * MVP.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragUV = texCoord;
}