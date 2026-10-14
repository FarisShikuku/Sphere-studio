#version 460
#include "include/vulkan_compat.glsl"
#include "include/bindless.glsl"
#include "include/pbr_common.glsl"

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoord;

layout(set = SCENE_SET, binding = 0) uniform SceneData {
    mat4 view;
    mat4 proj;
    vec4 cameraPos;
    vec4 sunDirection; // xyz = dir, w = intensity
    vec4 sunColor;     // xyz = rgb, w = ambient
} scene;

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

    vec3 direct = EvaluateDirectPBR(N, V, L, albedoSample.rgb, roughness, metallic, radiance);
    vec3 ambient = scene.sunColor.w * albedoSample.rgb * occlusion;

    outColor = vec4(direct + ambient, albedoSample.a);
}
