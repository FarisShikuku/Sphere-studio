// cascaded_shadows_common.glsl - Directional Cascaded Shadow Maps (CSM)
// Referencing Gaukler/PlainRenderer and SaschaWillems/Vulkan

struct GpuCascadedShadowData {
    mat4 cascadeViewProj[4];
    vec4 cascadeSplitDistances;  // c0, c1, c2, c3
    vec4 shadowFilterParams;     // x = pcfKernelSize, y = shadowMapRes, z = normalBias, w = depthBias
};

// Selects which cascade index a view-space Z depth falls into
int selectCascadeIndex(float viewDepth, vec4 splitDistances) {
    if (viewDepth < splitDistances.x) return 0;
    if (viewDepth < splitDistances.y) return 1;
    if (viewDepth < splitDistances.z) return 2;
    return 3;
}

// 5x5 Percentage Closer Filtering (PCF) on selected shadow cascade
float sampleCascadeShadowPCF(
    sampler2DArray shadowMapArray,
    GpuCascadedShadowData csm,
    vec3 worldPos,
    vec3 normal,
    float viewDepth
) {
    int cascadeIdx = selectCascadeIndex(viewDepth, csm.cascadeSplitDistances);

    // Normal offset bias to eliminate shadow acne on grazing slopes
    float normalBias = csm.shadowFilterParams.z;
    float depthBias = csm.shadowFilterParams.w;
    vec3 biasedWorldPos = worldPos + normal * normalBias;

    vec4 lightClip = csm.cascadeViewProj[cascadeIdx] * vec4(biasedWorldPos, 1.0);
    vec3 shadowCoords = lightClip.xyz / lightClip.w;

    // Transform to [0, 1] texture coordinates
    shadowCoords.xy = shadowCoords.xy * 0.5 + 0.5;

    if (shadowCoords.z > 1.0 || shadowCoords.x < 0.0 || shadowCoords.x > 1.0 || shadowCoords.y < 0.0 || shadowCoords.y > 1.0) {
        return 1.0; // Outside shadow map frustum
    }

    float shadowFactor = 0.0;
    float texelSize = 1.0 / csm.shadowFilterParams.y;
    float kernelRadius = csm.shadowFilterParams.x;

    for (int y = -2; y <= 2; ++y) {
        for (int x = -2; x <= 2; ++x) {
            vec2 offset = vec2(float(x), float(y)) * texelSize * (kernelRadius * 0.5);
            float shadowDepth = texture(shadowMapArray, vec3(shadowCoords.xy + offset, float(cascadeIdx))).r;
            if (shadowCoords.z - depthBias <= shadowDepth) {
                shadowFactor += 1.0;
            }
        }
    }

    return shadowFactor / 25.0;
}
