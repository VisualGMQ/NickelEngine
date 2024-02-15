#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform sampler mySampler;
layout(binding = 3) uniform texture2D myTexture;

void main() {
    outColor = texture(sampler2D(myTexture, mySampler), fragUV) * vec4(fragColor, 1.0);
}