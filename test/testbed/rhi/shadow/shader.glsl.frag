#version 450

in VS_OUT {
    vec3 normal;
    vec4 fragPos;
    vec2 uv;
} fs_in;

out vec4 outColor;

vec3 lightDir = normalize(vec3(-1, -1, -0.4));

layout(binding = 1) uniform sampler2D mySampler;

void main() {
    float factor = max(0, dot(-lightDir, fs_in.normal));

    vec4 fragPos = fs_in.fragPos / fs_in.fragPos.w;
    // OpenGL depth in [-1, 1] in NDC, so we tranfer it to [0, 1]
    fragPos = fragPos * 0.5 + 0.5;
    float depth = texture(mySampler, fragPos.xy).r;
    float currentDepth = fragPos.z;
    float bias = max(0.005 * (1.0 - dot(fs_in.normal, -lightDir)), 0.001);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(mySampler, 0);
    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            float pcfDepth = texture(mySampler, fragPos.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth + bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    outColor = factor * vec4(0, 1, 1, 1) * shadow;
}