#ifndef FENCE_MATERIAL_GLSL
#define FENCE_MATERIAL_GLSL

vec3 evalFenceSurfaceAlbedo(vec2 uv, uint materialMode) {
    if (materialMode == 0u) { // Corten weathering steel
        float grain = fract(sin(dot(floor(uv * 24.0), vec2(12.9898, 78.233))) * 43758.5453);
        return mix(vec3(0.45, 0.16, 0.06), vec3(0.68, 0.25, 0.10), grain);
    }
    if (materialMode == 1u) { // Western cedar timber slats
        float streak = 0.88 + 0.12 * sin(uv.x * 48.0);
        return vec3(0.71, 0.33, 0.04) * streak;
    }
    return vec3(0.55, 0.84, 0.96); // Tempered architectural glass
}

#endif // FENCE_MATERIAL_GLSL
