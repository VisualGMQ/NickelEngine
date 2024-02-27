#version 450

// layout(location = 0) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

// layout(binding = 1) uniform sampler2D mySampler;

void main() {
    // outColor = texture(mySampler, fragUV);
    // outColor = vec4(fragUV, 0, 1);
    outColor = vec4(1, 0, 0, 1);
}