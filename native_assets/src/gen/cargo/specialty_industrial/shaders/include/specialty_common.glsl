// Specialty industrial goods shared GLSL compute utilities
#ifndef SPECIALTY_COMMON_GLSL
#define SPECIALTY_COMMON_GLSL

float SpecialtyHash(vec2 p) {
    p = fract(p * vec2(678.91, 456.23));
    p += dot(p, p + 91.23);
    return fract(p.x * p.y);
}

float SpecialtyNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(SpecialtyHash(i), SpecialtyHash(i + vec2(1.0, 0.0)), f.x),
               mix(SpecialtyHash(i + vec2(0.0, 1.0)), SpecialtyHash(i + vec2(1.0, 1.0)), f.x), f.y);
}

// Rubber tyre tread sipes and blocks
float TyreTreadPattern(vec2 uv) {
    float chevron = abs(fract(uv.x * 16.0 + abs(uv.y - 0.5) * 4.0) - 0.5);
    return smoothstep(0.18, 0.28, chevron);
}

#endif
