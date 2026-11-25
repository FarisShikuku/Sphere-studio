// vegetation_common.glsl - Shared GLSL definitions for botanical synthesis
#ifndef VEGETATION_COMMON_GLSL
#define VEGETATION_COMMON_GLSL

struct VegetationVertex {
    vec4 position_lod;     // xyz = world pos, w = LOD factor [0..1]
    vec4 normal_ao;        // xyz = surface normal, w = ambient occlusion
    vec4 uv_material;      // xy = UV coordinates, z = subsurface scattering, w = wind flex
    vec4 color_roughness;  // rgb = albedo tint, a = roughness
};

vec3 evaluateWindDisplacement(vec3 worldPos, vec3 windDir, float windStrength, float flexFactor, float timeSec) {
    float wavePhase = dot(worldPos.xz, windDir.xz * 0.2) + timeSec * 2.8;
    float wave = sin(wavePhase) * 0.7 + sin(wavePhase * 2.3 + 1.2) * 0.3;
    return windDir * (wave * windStrength * flexFactor * flexFactor);
}

#endif // VEGETATION_COMMON_GLSL
