#version 450
#include "include/building_common.glsl"

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inWorldNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) flat in uint inMaterialId;

layout(push_constant) uniform LightingPushConstants {
    vec4 sunDirIntensity;
    vec4 cameraPosExposure;
    vec4 glassIorParams; // x = windowIor (1.0..2.8), y = reflBoost, z = privacyBias, w = reserved
} pc;

layout(location = 0) out vec4 outColor;

uint mapMaterialSlotToGlassType(uint matId) {
    if (matId == 7u) return 1u;  // GlassTinted
    if (matId == 8u) return 2u;  // GlassReflective
    if (matId == 9u) return 5u;  // GlassFrosted
    if (matId == 10u) return 3u; // GlassEmerald
    if (matId == 11u) return 6u; // GlassGold
    if (matId == 13u) return 4u; // GlassBronze
    if (matId == 14u) return 7u; // GlassStained
    return 0u;                   // ClearFloat / WallSpandrelGlass / BalconyGlass
}

void main() {
    vec3 N = normalize(inWorldNormal);
    vec3 L = normalize(pc.sunDirIntensity.xyz);
    vec3 V = normalize(pc.cameraPosExposure.xyz - inWorldPos);

    if (inMaterialId == 1u || (inMaterialId >= 7u && inMaterialId <= 14u) || inMaterialId == 16u || inMaterialId == 34u) {
        float ior = (pc.glassIorParams.x > 0.5) ? pc.glassIorParams.x : 1.85;
        float glassAlpha = 1.0;
        vec3 glassCol = evaluateRefractiveArchitecturalGlassPBR(
            mapMaterialSlotToGlassType(inMaterialId),
            inWorldPos,
            N,
            V,
            L,
            inUV,
            ior,
            glassAlpha
        );
        if (inMaterialId == 34u) {
            glassAlpha = 1.0;
        }
        outColor = vec4(glassCol * max(pc.sunDirIntensity.w, 0.35), glassAlpha);
        return;
    }

    float ndotl = max(dot(N, L), 0.0);
    vec3 albedo = vec3(0.75, 0.76, 0.78);
    if (inMaterialId == 20u) {
        albedo = computeProceduralBrickMortar(inUV, 12.0, 6.0, vec3(0.55, 0.22, 0.17), vec3(0.72, 0.69, 0.65));
    }

    vec3 lit = albedo * (0.28 + 0.72 * ndotl * pc.sunDirIntensity.w);
    outColor = vec4(lit, 1.0);
}
