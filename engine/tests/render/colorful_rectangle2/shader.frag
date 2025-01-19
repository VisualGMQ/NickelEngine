#version 450

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform FragColor {
    vec3 color;
}fragColor;

void main() {
    outColor = vec4(fragColor.color, 1.0);
}