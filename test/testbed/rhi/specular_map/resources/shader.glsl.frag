#version 450

in VS_OUT {
    vec2 fragUV;
    vec3 normal;
    vec3 fragPos;
} fs_in;

out vec4 outColor;

layout(binding = 1) uniform sampler2D mySampler;
layout(binding = 2) uniform sampler2D specularSampler;

layout(binding = 3) uniform EyePos {
    vec3 eyePos;
} eyePos;

const vec3 lightDir = normalize(vec3(0, -1, -1));
const float specularStrength = 0.5;
const int specularExp = 32;
const float ambient = 0.01;
const float diffuseStrength = 0.5;
const float diffuseFactor = 0.1;

void main() {
    vec3 textureColor = texture(mySampler, fs_in.fragUV).rgb;
    vec3 viewDir = normalize(eyePos.eyePos - fs_in.fragPos);
    vec3 reflDir = reflect(lightDir, fs_in.normal);
    float spec = pow(max(dot(viewDir, reflDir), 0.0), specularExp) * specularStrength;
    vec3 specColor = spec * texture(specularSampler, fs_in.fragUV).rgb;
    vec3 diffuseColor = max(dot(-lightDir, fs_in.normal), 0) * diffuseStrength * textureColor;
    vec3 ambientColor = ambient * textureColor;
    outColor = vec4(ambientColor + diffuseColor + specColor, 1.0);
}