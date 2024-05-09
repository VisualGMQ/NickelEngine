#version 430

layout(location = 0) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D positionSampler;
layout(binding = 1) uniform sampler2D normalSampler;

#define MaxLightNum 32

struct LightInfo {
    vec3 pos;
    vec3 color;
};

layout(binding = 2) uniform UBO {
    LightInfo lights[MaxLightNum];
} ubo;

const float kc = 1.0;
const float kl = 5;
const float kq = 10;

void main() {
    vec3 position = texture(positionSampler, fragUV).rgb;
    vec3 normal = texture(normalSampler, fragUV).rgb;

    if (normal == vec3(0.0)) {
        outColor = vec4(0, 0, 0, 1);
    } else {
        vec3 color = vec3(0.0);

        for (int i = 0; i < MaxLightNum; i++) {
            vec3 l = ubo.lights[i].pos - position;
            float dist = length(l);
            l = normalize(l);
            float attnuence = 1.0 / (kc + kl * dist + kq * dist * dist);
            color += ubo.lights[i].color * attnuence * max(dot(l, normal), 0.0);
        }

        outColor = vec4(color, 1.0);
    }
}