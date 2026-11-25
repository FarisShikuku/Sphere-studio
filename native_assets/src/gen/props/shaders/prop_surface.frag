#version 450
#extension GL_EXT_ray_query : require
#include "include/prop_pbr.glsl"

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inWorldNormal;
layout(location = 2) in vec4 inColor;

struct PropLight3D {
    vec4 posRadius;      // xyz = emitter position, w = bulbRadius
    vec4 dirRange;       // xyz = beam axis, w = maxRange
    vec4 colorFlux;      // rgb = linear blackbody color, w = luminousFluxLm
    vec4 spotCosAndVol;  // x = cosInner, y = cosOuter, z = volumetricStrength, w = shadowBias
};

layout(set = 0, binding = 0) uniform accelerationStructureEXT uPropTLAS;
layout(set = 0, binding = 1) readonly buffer PropLightBuffer {
    uint activeLightCount;
    PropLight3D lights[];
};

layout(push_constant) uniform PropLightPush {
    vec4 sunDirIntensity;
    vec4 cameraPosRoughness;
} pc;

layout(location = 0) out vec4 outFragColor;

// SaschaWillems/Vulkan raytracingshadows normal-offset BVH shadow query
float tracePropShadowRay(vec3 origin, vec3 normal, vec3 dir, float maxDist) {
    rayQueryEXT rq;
    vec3 biasedOrig = origin + normal * 0.0045;
    rayQueryInitializeEXT(rq, uPropTLAS, gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT, 0xFF, biasedOrig, 0.002, dir, maxDist);
    rayQueryProceedEXT(rq);
    return (rayQueryGetIntersectionTypeEXT(rq, true) != gl_RayIntersectionNoneEXT) ? 0.0 : 1.0;
}

void main() {
    vec3 N = normalize(inWorldNormal);
    vec3 Lsun = normalize(pc.sunDirIntensity.xyz);
    vec3 V = normalize(pc.cameraPosRoughness.xyz - inWorldPos);
    float roughness = clamp(pc.cameraPosRoughness.w, 0.04, 1.0);

    float sunVis = tracePropShadowRay(inWorldPos, N, Lsun, 80.0);
    vec3 lit = evalPropSurfaceLighting(inColor.rgb, N, Lsun, V, 0.25, roughness) * (pc.sunDirIntensity.w * sunVis);
    lit += inColor.rgb * 0.22;

    uint nLights = min(activeLightCount, 8u);
    for (uint i = 0u; i < nLights; ++i) {
        PropLight3D pl = lights[i];
        vec3 toL = pl.posRadius.xyz - inWorldPos;
        float dist = length(toL);
        if (dist > pl.dirRange.w) continue;
        vec3 L = toL / max(dist, 1e-4);

        float atten = evalPropWindowedInvSquare(dist, pl.posRadius.w, pl.dirRange.w);
        float cone = evalPropSpotCone(L, pl.dirRange.xyz, pl.spotCosAndVol.x, pl.spotCosAndVol.y);
        float vis = tracePropShadowRay(inWorldPos, N, L, dist - 0.02);
        float cd = pl.colorFlux.w / max(0.35, 2.0 * PI_PROP * (1.0 - pl.spotCosAndVol.y));
        lit += evalPropSurfaceLighting(inColor.rgb, N, L, V, 0.25, roughness) * pl.colorFlux.rgb * (cd * 0.015 * atten * cone * vis);
    }

    outFragColor = vec4(lit, inColor.a);
}
