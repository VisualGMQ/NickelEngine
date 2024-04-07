#version 450

layout(location = 0) in VS_OUT {
    vec2 fragUV;
    vec3 inPos;
    vec3 fragPos;
    mat3 TBN;
} fs_in;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform MyMaterial {
    vec4 baseColor;
    float metalness;
    float roughness;
} Material;

layout(binding = 2) uniform sampler2D baseColorSampler;
layout(binding = 3) uniform sampler2D normalMapSampler;
layout(binding = 4) uniform sampler2D metalroughnessSampler;
layout(binding = 5) uniform samplerCube skyboxSampler;

layout(binding = 6) uniform MyCameraInfo {
    vec3 eyePos;
} CameraInfo;

const vec3 lightDir = normalize(vec3(-1, -1, -1));
const float shininess = 64;

const float diffuseStrength = 0.2;
const float specularStrength = 0.6;
const float ambientStrength = 0.2;

const float perceptualRoughness = 0.3;
const float PI = 3.141592653589;

float D_GGX(float NoH, float roughness) {
    float a = NoH * roughness;
    float k = roughness / (1.0 - NoH * NoH + a * a);
    return k * k * (1.0 / PI);
}

float V_SmithGGXCorrelatedFast(float NoV, float NoL, float roughness) {
    float a = roughness;
    float GGXV = NoL * (NoV * (1.0 - a) + a);
    float GGXL = NoV * (NoL * (1.0 - a) + a);
    return 0.5 / (GGXV + GGXL);
}

vec3 F_Schlick(float VoH, vec3 f0) {
    float f = pow(1.0 - VoH, 5.0);
    return f + f0 * (1.0 - f);
}

float Fd_Lambert() {
    return 1.0 / PI;
}

void main() {
    vec3 n = texture(normalMapSampler, fs_in.fragUV).rgb;
    n = normalize(n * 2.0 - 1.0);
    n = normalize(fs_in.TBN * n);

    vec3 v = normalize(CameraInfo.eyePos - fs_in.fragPos);
    vec3 l = -lightDir;

    vec3 baseColor = texture(baseColorSampler, fs_in.fragUV).rgb;
    vec3 envMapColor = texture(skyboxSampler, normalize(fs_in.inPos)).rgb;

    vec3 h = normalize(v + l);

    float NoV = abs(dot(n, v)) + 1e-5;
    float NoL = clamp(dot(n, l), 0.0, 1.0);
    float NoH = clamp(dot(n, h), 0.0, 1.0);
    float LoH = clamp(dot(l, h), 0.0, 1.0);

    float roughness = texture(metalroughnessSampler, fs_in.fragUV).g;
    float metalness = texture(metalroughnessSampler, fs_in.fragUV).b;

    vec3 f0 = metalness * baseColor * envMapColor;

    float D = D_GGX(NoH, roughness);
    vec3  F = F_Schlick(LoH, f0);
    float V = V_SmithGGXCorrelatedFast(NoV, NoL, roughness);

    vec3 DFV = D * V * F;
    vec3 energyCompensation = 1.0 + f0 * (1.0 / DFV.y - 1.0);

    vec3 Fr = DFV * energyCompensation;
    vec3 Fd = (1 - F) * baseColor * Fd_Lambert();

    outColor = vec4(Fd + Fr / (4 * dot(n, v), dot(n, l) + 0.0001), 1.0) * max(dot(n, l), 0);
}