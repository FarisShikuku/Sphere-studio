#ifndef PBR_COMMON_GLSL
#define PBR_COMMON_GLSL
#include "math_common.glsl"

// GGX / Trowbridge-Reitz Normal Distribution Function
float D_GGX(float NdotH, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / max(PI * denom * denom, 1e-7);
}

// Smith GGX Correlated Visibility / Geometry Function
float V_SmithGGXCorrelated(float NdotV, float NdotL, float roughness) {
    float a = roughness * roughness;
    float GGXV = NdotL * sqrt(NdotV * (NdotV - a * NdotV) + a);
    float GGXL = NdotV * sqrt(NdotL * (NdotL - a * NdotL) + a);
    return 0.5 / max(GGXV + GGXL, 1e-7);
}

// Schlick Fresnel Approximation
vec3 F_Schlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 F_SchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Standard PBR Cook-Torrance evaluation
vec3 EvaluateDirectPBR(vec3 N, vec3 V, vec3 L, vec3 albedo, float roughness, float metallic, vec3 radiance) {
    vec3 H = normalize(V + L);
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 1e-5);
    float NdotH = max(dot(N, H), 0.0);
    float LdotH = max(dot(L, H), 0.0);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    float D = D_GGX(NdotH, roughness);
    float Vis = V_SmithGGXCorrelated(NdotV, NdotL, roughness);
    vec3 F = F_Schlick(LdotH, F0);

    vec3 specular = D * Vis * F;
    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
    vec3 diffuse = kD * albedo * INV_PI;

    return (diffuse + specular) * radiance * NdotL;
}

#endif // PBR_COMMON_GLSL
