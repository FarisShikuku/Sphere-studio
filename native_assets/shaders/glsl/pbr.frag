#version 460
#include "include/vulkan_compat.glsl"
#include "include/bindless.glsl"
#include "include/pbr_common.glsl"

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec4 inShadowCoord; // Light-space projected coordinate

layout(set = SCENE_SET, binding = 0) uniform SceneData {
    mat4 view;
    mat4 proj;
    vec4 cameraPos;
    vec4 sunDirection; // xyz = dir, w = intensity
    vec4 sunColor;     // xyz = rgb, w = ambient
    vec4 shadowParams; // x = enabled, y = bias, z = pcfRange, w = shadowMapResolution
} scene;

// Shadow map depth attachment sampler (Set 0, Binding 1)
// Reference: SaschaWillems/Vulkan (shadowmapping/scene.frag)
layout(set = SCENE_SET, binding = 1) uniform sampler2D shadowMap;

layout(push_constant) uniform MaterialPush {
    layout(offset = 128)
    vec4 baseColorFactor;
    float metallicFactor;
    float roughnessFactor;
    uint albedoTexIndex;
    uint normalTexIndex;
    uint ormTexIndex;
} mat;

layout(location = 0) out vec4 outColor;

// Projective texture sampling with depth comparison
// Reference: SaschaWillems/Vulkan shadowmapping/scene.frag
float textureProj(vec4 shadowCoord, vec2 off, float bias) {
    float shadow = 1.0;
    if (shadowCoord.z > -1.0 && shadowCoord.z < 1.0) {
        float dist = texture(shadowMap, shadowCoord.st + off).r;
        if (shadowCoord.w > 0.0 && dist < shadowCoord.z - bias) {
            shadow = 0.0;
        }
    }
    return shadow;
}

// Multi-tap Percentage Closer Filtering (PCF)
// Reference: SaschaWillems/Vulkan shadowmapping/scene.frag (filterPCF)
float filterPCF(vec4 sc, float bias, int range) {
    ivec2 texDim = textureSize(shadowMap, 0);
    float scale = 1.5;
    float dx = scale * 1.0 / float(texDim.x);
    float dy = scale * 1.0 / float(texDim.y);

    float shadowFactor = 0.0;
    int count = 0;

    for (int x = -range; x <= range; x++) {
        for (int y = -range; y <= range; y++) {
            shadowFactor += textureProj(sc, vec2(dx * float(x), dy * float(y)), bias);
            count++;
        }
    }
    return (count > 0) ? (shadowFactor / float(count)) : 1.0;
}

void main() {
    vec4 albedoSample = mat.baseColorFactor;
    if (mat.albedoTexIndex != 0) {
        albedoSample *= SampleBindless2D(mat.albedoTexIndex, inTexCoord);
    }

    vec3 N = normalize(inNormal);
    if (mat.normalTexIndex != 0) {
        mat3 TBN = ConstructTBN(inNormal, inTangent);
        vec3 rawNormal = SampleBindless2D(mat.normalTexIndex, inTexCoord).rgb;
        N = normalize(TBN * (rawNormal * 2.0 - 1.0));
    }

    float metallic = mat.metallicFactor;
    float roughness = mat.roughnessFactor;
    float occlusion = 1.0;
    if (mat.ormTexIndex != 0) {
        vec3 orm = SampleBindless2D(mat.ormTexIndex, inTexCoord).rgb;
        occlusion = orm.r;
        roughness *= orm.g;
        metallic *= orm.b;
    }

    vec3 V = normalize(scene.cameraPos.xyz - inWorldPos);
    vec3 L = normalize(scene.sunDirection.xyz);
    vec3 radiance = scene.sunColor.xyz * scene.sunDirection.w;

    // Calculate dynamic slope-scaled depth bias to prevent shadow acne
    // Reference: SaschaWillems/Vulkan shadowmapping
    float cosTheta = clamp(dot(N, L), 0.0, 1.0);
    float dynamicBias = max(scene.shadowParams.y * (1.0 - cosTheta), scene.shadowParams.y * 0.1);

    // Compute shadow factor (1.0 = full light, 0.0 = fully occluded)
    float shadow = 1.0;
    if (scene.shadowParams.x > 0.5 && cosTheta > 0.0) {
        int pcfRange = int(clamp(scene.shadowParams.z, 0.0, 3.0));
        if (pcfRange == 0) {
            shadow = textureProj(inShadowCoord / inShadowCoord.w, vec2(0.0), dynamicBias);
        } else {
            shadow = filterPCF(inShadowCoord / inShadowCoord.w, dynamicBias, pcfRange);
        }
    } else if (cosTheta <= 0.0) {
        shadow = 0.0; // Self-shadowing back faces
    }

    vec3 direct = EvaluateDirectPBR(N, V, L, albedoSample.rgb, roughness, metallic, radiance) * shadow;
    vec3 ambient = scene.sunColor.w * albedoSample.rgb * occlusion;

    outColor = vec4(direct + ambient, albedoSample.a);
}
