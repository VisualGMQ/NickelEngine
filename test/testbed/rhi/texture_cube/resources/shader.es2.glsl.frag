#version 300 es

precision mediump float;

in vec3 fragColor;
in vec2 fragUV;

layout(location = 0) out vec4 outColor;

uniform sampler2D mySampler;

void main() {
    outColor = texture(mySampler, fragUV) * vec4(fragColor, 1.0);
}