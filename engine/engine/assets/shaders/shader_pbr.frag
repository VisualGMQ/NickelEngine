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

layout(binding = 2) uniform texture2D baseColorTexture;
layout(binding = 3) uniform texture2D normalMapTexture;
layout(binding = 4) uniform texture2D metalroughnessTexture;
layout(binding = 5) uniform texture2D occlusionTexture;
layout(binding = 6) uniform sampler baseColorSampler;
layout(binding = 7) uniform sampler normalMapSampler;
layout(binding = 8) uniform sampler metalroughnessSampler;
layout(binding = 9) uniform sampler occlusionSampler;

layout(binding = 10) uniform MyCameraInfo {
    vec3 eyePos;
} CameraInfo;

const float PI = 3.14159265359;
const vec3 lightDir = normalize(vec3(1, 1, 1));

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 LambertAlbedo(vec3 f0) {
    return f0 / PI;
}

// void main() {
//     vec3 baseColor = texture(sampler2D(baseColorTexture, baseColorSampler), fs_in.fragUV).rgb * Material.baseColor.rgb;
//     outColor = vec4(baseColor, 1.0);
// }

void main() {		
    mat3 TBN = mat3(normalize(fs_in.TBN[0]),
                    normalize(fs_in.TBN[1]),
                    normalize(fs_in.TBN[2]));
    vec3 n = texture(sampler2D(normalMapTexture, normalMapSampler), fs_in.fragUV).rgb;
    vec3 baseColor = texture(sampler2D(baseColorTexture, baseColorSampler), fs_in.fragUV).rgb * Material.baseColor.rgb;
    float occlusion = texture(sampler2D(occlusionTexture, occlusionSampler), fs_in.fragUV).r;
    vec3 metalRoughness = texture(sampler2D(metalroughnessTexture, metalroughnessSampler), fs_in.fragUV).rgb;

    n = normalize(n * 2.0 - 1.0);
    n = normalize(TBN * n);

    vec3 v = normalize(CameraInfo.eyePos - fs_in.fragPos);
    vec3 l = -lightDir;

    vec3 h = normalize(v + l);

    float NoV = dot(n, v);
    float HoV = dot(h, v);
    float NoL = clamp(dot(n, l), 0.0, 1.0);
    float NoH = clamp(dot(n, h), 0.0, 1.0);
    float LoH = clamp(dot(l, h), 0.0, 1.0);

    float roughness = metalRoughness.g * Material.roughness;
    float metalness = metalRoughness.b * Material.metalness;

    vec3 f0 = metalness * baseColor;

    // calculate per-light radiance
    vec3 radiance = vec3(100, 100, 100);

    // Cook-Torrance BRDF
    float D = DistributionGGX(n, h, roughness);   
    float G = GeometrySmith(n, v, l, roughness);      
    vec3 F  = fresnelSchlick(clamp(HoV, 0.0, 1.0), f0);
        
    vec3 numerator    = D * G * F; 
    float denominator = 4.0 * max(NoV, 0.0) * max(NoL, 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metalness;	  

    vec3 albedo = baseColor * occlusion;

    // add to outgoing radiance Lo
    vec3 Lo = (kD * albedo / PI + specular) * radiance * max(NoL, 0.0);
    
    vec3 ambient = vec3(0.03) * albedo * occlusion;
    vec3 color = ambient + Lo;

    outColor = vec4(color, 1.0);
}