#version 300 es

precision mediump float;

in vec2 fragUV;
in vec3 normal;
in vec3 fragPos;

out vec4 outColor;

uniform sampler2D mySampler;
uniform sampler2D specularSampler;

uniform EyePos {
    vec3 eyePos;
} eyePos;

const vec3 lightDir = normalize(vec3(0, -1, -1));
const float specularStrength = 0.5;
const float specularExp = 32.0;
const float ambient = 0.01;
const float diffuseStrength = 0.5;
const float diffuseFactor = 0.1;

void main() {
    vec3 textureColor = texture(mySampler, fragUV).rgb;
    vec3 viewDir = normalize(eyePos.eyePos - fragPos);
    vec3 reflDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDir, reflDir), 0.0), specularExp) * specularStrength;
    vec3 specColor = spec * texture(specularSampler, fragUV).rgb;
    vec3 diffuseColor = max(dot(-lightDir, normal), 0.0) * diffuseStrength * textureColor;
    vec3 ambientColor = ambient * textureColor;
    outColor = vec4(ambientColor + diffuseColor + specColor, 1.0);
}