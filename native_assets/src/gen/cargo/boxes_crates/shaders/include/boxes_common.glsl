// Boxes and Crates shared GLSL compute utilities
#ifndef BOXES_COMMON_GLSL
#define BOXES_COMMON_GLSL

float BoxHash(vec2 p) {
    p = fract(p * vec2(234.34, 789.12));
    p += dot(p, p + 34.56);
    return fract(p.x * p.y);
}

float BoxNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(BoxHash(i), BoxHash(i + vec2(1.0, 0.0)), f.x),
               mix(BoxHash(i + vec2(0.0, 1.0)), BoxHash(i + vec2(1.0, 1.0)), f.x), f.y);
}

// Wood grain streaks along longitudinal axis
float WoodGrain(vec2 uv, float ringFrequency) {
    float n = BoxNoise(vec2(uv.x * 2.0, uv.y * 32.0));
    float dist = length(vec2(uv.x * 4.0, uv.y * 0.2)) + n * 0.08;
    return sin(dist * ringFrequency) * 0.5 + 0.5;
}

// Corrugated cardboard sinusoidal fluting
float CardboardCorrugation(vec2 uv, float pitch) {
    return sin(uv.x * 6.28318 / max(0.001, pitch));
}

#endif
