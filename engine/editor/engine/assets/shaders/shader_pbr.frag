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

// void main() {
//     vec3 baseColor = texture(sampler2D(baseColorTexture, baseColorSampler), fs_in.fragUV).rgb * Material.baseColor.rgb;
//     outColor = vec4(baseColor, 1.0);
// }

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------

const vec3 lightDir = vec3(-0.2, -0.6, -1);
const vec3 lightColor = vec3(1, 1, 1);

void main()
{
    mat3 TBN = mat3(normalize(fs_in.TBN[0]),
    normalize(fs_in.TBN[1]),
    normalize(fs_in.TBN[2]));
    vec3 N = texture(sampler2D(normalMapTexture, normalMapSampler), fs_in.fragUV).rgb;
    vec3 albedo = texture(sampler2D(baseColorTexture, baseColorSampler), fs_in.fragUV).rgb * Material.baseColor.rgb;
    float occlusion = texture(sampler2D(occlusionTexture, occlusionSampler), fs_in.fragUV).r;
    vec3 metalRoughness = texture(sampler2D(metalroughnessTexture, metalroughnessSampler), fs_in.fragUV).rgb;

    float roughness = metalRoughness.g * Material.roughness;
    float metallic = metalRoughness.b * Material.metalness;

    N = normalize(N * 2.0 - 1.0);
    N = normalize(TBN * N);
    
    vec3 V = normalize(CameraInfo.eyePos - fs_in.fragPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);

    // calculate per-light radiance
    vec3 L = normalize(-lightDir);
    vec3 H = normalize(V + L);
    float distance = length(lightDir);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = lightColor * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;

    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * occlusion;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    outColor = vec4(color, 1.0);
}