#ifndef MINIBUS_COMMON_GLSL
#define MINIBUS_COMMON_GLSL

struct MinibusPushConstants {
    vec4 baseColor;
    vec4 accentColor;
    uint seed;
    float roofRackLoad;
    float slidingDoorRatio;
    uint variantId;
};

float hashMinibus(vec2 p, uint seed) {
    return fract(sin(dot(p, vec2(12.9898, 78.233)) + float(seed)) * 43758.5453);
}

#endif
