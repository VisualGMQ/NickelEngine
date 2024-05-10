#version 450

layout(location = 0) in vec3 inPosition;

layout(location = 0) out VS_OUT{
    vec3 fragUV;
} vs_out;

layout(binding = 0) uniform MyUniform {
    mat4 view;
    mat4 proj;
} MVP;

void main() {
    gl_Position = MVP.proj * MVP.view * vec4(inPosition, 1.0);
    vs_out.fragUV = inPosition;
}