#ifndef NEON_EMISSIVE_GLSL
#define NEON_EMISSIVE_GLSL

vec3 evalNeonTubeEmission(float dist, vec3 tubeColor, float radius, float bloomSpread, float pulse) {
    float core = smoothstep(radius, 0.0, dist);
    float halo = exp(-dist * bloomSpread) * pulse;
    return mix(tubeColor * halo, vec3(1.0), core * 0.9);
}

#endif // NEON_EMISSIVE_GLSL
