// sdf_gi_common.glsl - Signed Distance Field Diffuse GI & Ambient Occlusion
// Referencing Gaukler/PlainRenderer continuous SDF volume cone-tracing

struct GpuSdfGiData {
    vec4 volumeMin;             // xyz = world-space AABB min, w = voxelSize
    vec4 volumeMax;             // xyz = world-space AABB max, w = maxRayDistance
    vec4 gridResolution;        // xyz = grid dimensions (e.g. 64, 64, 64), w = totalVoxels
    vec4 giParameters;          // x = bounceFactor, y = coneAngleRad, z = stepSizeFactor, w = aoIntensity
    vec4 colorBleedingParams;   // x = indirectColorBoost, y = normalOffsetBias, z = multiBounceApprox, w = debugViewMode
};

// Continuous 3D texture sampler for Signed Distance Field
// In Vulkan: layout(set = 1, binding = 0) uniform sampler3D u_sdfTexture;
// Here encapsulated for shader modularity

float sampleSdfVolume(sampler3D sdfTex, GpuSdfGiData gi, vec3 worldPos) {
    vec3 uvw = (worldPos - gi.volumeMin.xyz) / (gi.volumeMax.xyz - gi.volumeMin.xyz);
    if (any(lessThan(uvw, vec3(0.0))) || any(greaterThan(uvw, vec3(1.0)))) {
        return 10.0;
    }
    return texture(sdfTex, uvw).r;
}

// Distance Field Ambient Occlusion (DFAO)
float evaluateSdfAmbientOcclusion(sampler3D sdfTex, GpuSdfGiData gi, vec3 surfacePos, vec3 normal) {
    float occlusion = 0.0;
    float steps[4] = float[4](0.05, 0.12, 0.25, 0.45);

    for (int i = 0; i < 4; ++i) {
        float stepDist = steps[i];
        vec3 p = surfacePos + normal * stepDist;
        float actualDist = sampleSdfVolume(sdfTex, gi, p);
        float diff = max(0.0, stepDist - actualDist);
        occlusion += (diff / stepDist) * (1.0 / float(1 + i));
    }

    return 1.0 - clamp(occlusion * gi.giParameters.w, 0.0, 1.0);
}

// Cone-traced diffuse indirect irradiance
vec3 coneTraceIndirectDiffuse(sampler3D sdfTex, sampler3D albedoTex, GpuSdfGiData gi, vec3 surfacePos, vec3 normal) {
    vec3 up = abs(normal.y) < 0.99 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = normalize(cross(normal, tangent));

    // 6 Cosine-weighted hemisphere cone directions
    vec3 coneDirs[6] = vec3[6](
        vec3(0.0, 1.0, 0.0),
        vec3(0.8, 0.5, 0.0),
        vec3(-0.8, 0.5, 0.0),
        vec3(0.0, 0.5, 0.8),
        vec3(0.0, 0.5, -0.8),
        vec3(0.577, 0.577, 0.577)
    );

    vec3 accumulatedRadiance = vec3(0.0);
    float totalWeight = 0.0;
    float startOffset = gi.colorBleedingParams.y; // Normal offset to clear starting surface
    float maxDist = gi.volumeMax.w;
    float stepFactor = gi.giParameters.z;

    for (int i = 0; i < 6; ++i) {
        vec3 localDir = coneDirs[i];
        vec3 worldDir = normalize(tangent * localDir.x + normal * localDir.y + bitangent * localDir.z);
        float cosTheta = max(0.0, dot(worldDir, normal));
        if (cosTheta <= 0.0) continue;

        float t = startOffset;
        for (int step = 0; step < 16; ++step) {
            vec3 samplePos = surfacePos + worldDir * t;
            float dist = sampleSdfVolume(sdfTex, gi, samplePos);

            if (dist < 0.04) {
                // Hit surface in distance field: gather bouncing albedo
                vec3 uvw = (samplePos - gi.volumeMin.xyz) / (gi.volumeMax.xyz - gi.volumeMin.xyz);
                vec3 bounceColor = texture(albedoTex, uvw).rgb;
                accumulatedRadiance += bounceColor * cosTheta;
                break;
            }

            t += max(dist * stepFactor, 0.05);
            if (t > maxDist) break;
        }
        totalWeight += cosTheta;
    }

    if (totalWeight > 0.001) {
        accumulatedRadiance /= totalWeight;
    }

    return accumulatedRadiance * gi.giParameters.x * gi.colorBleedingParams.x;
}
