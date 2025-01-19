#version 450

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform FragColor {
    vec3 color;
}fragColor;

void main() {
    outColor = vec4(fragColor.color, 1.0);
}