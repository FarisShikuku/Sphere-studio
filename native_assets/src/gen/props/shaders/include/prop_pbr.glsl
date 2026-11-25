#ifndef PROP_PBR_GLSL
#define PROP_PBR_GLSL

const float PI_PROP = 3.14159265359;

vec3 evalPropFresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Frostbite windowed inverse-square attenuation
float evalPropWindowedInvSquare(float dist, float bulbRadius, float maxRange) {
    float safeD = max(dist, max(bulbRadius, 0.08));
    float r = dist / max(maxRange, 0.5);
    float win = clamp(1.0 - r * r * r * r, 0.0, 1.0);
    return (win * win) / (safeD * safeD);
}

// 3D solid-angle spot light beam falloff
float evalPropSpotCone(vec3 L_toLight, vec3 spotDir, float cosInner, float cosOuter) {
    return smoothstep(cosOuter, max(cosOuter + 0.02, cosInner), dot(-L_toLight, normalize(spotDir)));
}

// Henyey-Greenstein 3D anisotropic forward scattering phase function
float evalPropHGPhase(float cosTheta, float g) {
    float g2 = g * g;
    float denom = max(0.05, 1.0 + g2 - 2.0 * g * cosTheta);
    return (1.0 - g2) / (4.0 * PI_PROP * denom * sqrt(denom));
}

// Beer-Lambert participating medium transmittance T(s) = exp(-sigma_t * s)
float evalPropBeerLambert(float sigmaT, float dist) {
    return exp(-max(0.0, sigmaT) * max(0.0, dist));
}

// Khronos / SaschaWillems Cook-Torrance microfacet PBR evaluation
vec3 evalPropSurfaceLighting(vec3 albedo, vec3 N, vec3 L, vec3 V, float metallic, float roughness) {
    vec3 H = normalize(L + V);
    float ndotl = max(dot(N, L), 0.0);
    float ndotv = max(dot(N, V), 0.001);
    float ndoth = max(dot(N, H), 0.0);
    float vdoth = max(dot(V, H), 0.0);

    float r = clamp(roughness, 0.04, 1.0);
    float alpha = r * r;
    float alpha2 = alpha * alpha;
    float denom = ndoth * ndoth * (alpha2 - 1.0) + 1.0;
    float D = alpha2 / (PI_PROP * denom * denom + 1e-6);

    float k = (r + 1.0) * (r + 1.0) * 0.125;
    float G = (ndotv / (ndotv * (1.0 - k) + k)) * (ndotl / (ndotl * (1.0 - k) + k));

    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = evalPropFresnelSchlick(vdoth, F0);
    vec3 spec = (D * G * F) / (4.0 * ndotv * ndotl + 1e-4);
    vec3 kd = (1.0 - F) * (1.0 - metallic);
    return (kd * albedo / PI_PROP + spec) * ndotl;
}

#endif // PROP_PBR_GLSL
