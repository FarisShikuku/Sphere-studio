#version 450
// plain_renderer_pbr.frag - Full PlainRenderer PBR Forward/Deferred Lighting Shader
// Reference: Gaukler/PlainRenderer Vulkan Graphics Pipeline

#include "include/lighting_common.glsl"
#include "include/atmosphere_common.glsl"
#include "include/sdf_gi_common.glsl"
#include "include/cascaded_shadows_common.glsl"

layout(location = 0) in vec2 inUV;
layout(location = 0) out vec4 outFinalColor;

// UBOs
layout(set = 0, binding = 0) uniform FrameUBO {
    mat4 u_view;
    mat4 u_proj;
    mat4 u_invViewProj;
    vec4 u_cameraPosition; // xyz = pos, w = viewDistance
    int  u_debugViewMode;  // 0=Final, 1=Direct Only, 2=Indirect GI Only, 3=SDF Voxels, 4=Cascades, 5=Sky Only, 6=DFAO
};

layout(set = 0, binding = 1) uniform LightUBO {
    GpuLightData u_primaryLight;
};

layout(set = 0, binding = 2) uniform AtmosphereUBO {
    GpuAtmosphereData u_atmosphere;
};

layout(set = 0, binding = 3) uniform SdfGiUBO {
    GpuSdfGiData u_sdfGi;
};

layout(set = 0, binding = 4) uniform CascadedShadowUBO {
    GpuCascadedShadowData u_cascadedShadows;
};

// Textures
layout(set = 1, binding = 0) uniform sampler2D u_albedoRoughness; // RGB = Albedo, A = Roughness
layout(set = 1, binding = 1) uniform sampler2D u_normalMetallic;  // RGB = Normal, A = Metallic
layout(set = 1, binding = 2) uniform sampler2D u_depthTexture;
layout(set = 1, binding = 3) uniform sampler2D u_indirectGiTexture; // RGB = Indirect GI, A = DFAO
layout(set = 1, binding = 4) uniform sampler2DArray u_shadowMapArray;
layout(set = 1, binding = 5) uniform sampler3D u_sdfTexture;

vec3 reconstructWorldPosition(vec2 uv, float depth) {
    vec4 clip = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 world = u_invViewProj * clip;
    return world.xyz / world.w;
}

void main() {
    float depth = texture(u_depthTexture, inUV).r;

    // Background skybox
    if (depth >= 1.0) {
        vec3 worldPos = reconstructWorldPosition(inUV, 1.0);
        vec3 viewRay = normalize(worldPos - u_cameraPosition.xyz);
        vec3 skyColor = evaluateSkyRadiance(u_atmosphere, viewRay);
        outFinalColor = vec4(skyColor, 1.0);
        return;
    }

    vec4 albRough = texture(u_albedoRoughness, inUV);
    vec4 normMetal = texture(u_normalMetallic, inUV);
    vec4 indirectGiAndAo = texture(u_indirectGiTexture, inUV);

    vec3 albedo = albRough.rgb;
    float roughness = max(albRough.a, 0.04);
    vec3 normal = normalize(normMetal.rgb);
    float metallic = normMetal.a;

    vec3 worldPos = reconstructWorldPosition(inUV, depth);
    vec3 V = normalize(u_cameraPosition.xyz - worldPos);
    float viewDist = length(u_cameraPosition.xyz - worldPos);

    // 1. Direct Illumination (Flashlight / Bulb / Area / Sun)
    vec3 directIllum = evaluateLightIllumination(
        u_primaryLight,
        worldPos,
        normal,
        V,
        albedo,
        roughness,
        metallic
    );

    // 2. Cascaded Shadow Attenuation (Directional Sun) or Punctual Shadow
    float shadow = 1.0;
    if (u_primaryLight.areaParams.w > 0.5) { // castShadows enabled
        if (int(u_primaryLight.positionAndType.w) == 0) {
            // Directional Sun CSM
            shadow = sampleCascadeShadowPCF(u_shadowMapArray, u_cascadedShadows, worldPos, normal, viewDist);
        } else {
            // Spot / Point simple shadow test
            vec4 lightClip = u_primaryLight.lightSpaceMatrix * vec4(worldPos, 1.0);
            vec3 sc = (lightClip.xyz / lightClip.w) * 0.5 + 0.5;
            if (sc.x >= 0.0 && sc.x <= 1.0 && sc.y >= 0.0 && sc.y <= 1.0 && sc.z <= 1.0) {
                float shadowDepth = texture(u_shadowMapArray, vec3(sc.xy, 0.0)).r;
                shadow = (sc.z - 0.005 <= shadowDepth) ? 1.0 : 0.15;
            }
        }
    }
    vec3 directLit = directIllum * shadow;

    // 3. Indirect Diffuse GI (SDF Cone-Traced Bounce)
    vec3 indirectDiffuseRadiance = indirectGiAndAo.rgb;
    float dfao = indirectGiAndAo.a;

    // Indirect specular reflection from sky hemisphere
    vec3 R = reflect(-V, normal);
    vec3 skyAmbient = evaluateSkyRadiance(u_atmosphere, normal) * 0.4;
    vec3 indirectDiffuse = (indirectDiffuseRadiance + skyAmbient) * albedo * dfao;

    // 4. Combined Radiance
    vec3 finalColor = directLit + indirectDiffuse;

    // 5. Debug Visualizations matching PlainRenderer
    if (u_debugViewMode == 1) {
        // Direct light only
        finalColor = directLit;
    } else if (u_debugViewMode == 2) {
        // Indirect GI Bounce only
        finalColor = indirectDiffuseRadiance;
    } else if (u_debugViewMode == 3) {
        // Distance Field Voxels
        float sdfDist = sampleSdfVolume(u_sdfTexture, u_sdfGi, worldPos);
        finalColor = (sdfDist < 0.0) ? vec3(0.9, 0.1, 0.1) : vec3(clamp(sdfDist * 0.5, 0.0, 1.0));
    } else if (u_debugViewMode == 4) {
        // Cascades Debug: tint by cascade index (0=Red, 1=Green, 2=Blue, 3=Yellow)
        int cascadeIdx = selectCascadeIndex(viewDist, u_cascadedShadows.cascadeSplitDistances);
        vec3 cascadeTints[4] = vec3[4](
            vec3(1.0, 0.25, 0.25),
            vec3(0.25, 1.0, 0.25),
            vec3(0.25, 0.4, 1.0),
            vec3(1.0, 1.0, 0.2)
        );
        finalColor = directLit * cascadeTints[cascadeIdx];
    } else if (u_debugViewMode == 5) {
        // Atmospheric Sky Only
        finalColor = evaluateSkyRadiance(u_atmosphere, normal);
    } else if (u_debugViewMode == 6) {
        // DFAO Only
        finalColor = vec3(dfao);
    }

    // Tone Mapping & Gamma Correction
    finalColor = finalColor / (finalColor + vec3(1.0));
    finalColor = pow(finalColor, vec3(1.0 / 2.2));

    outFinalColor = vec4(finalColor, 1.0);
}
