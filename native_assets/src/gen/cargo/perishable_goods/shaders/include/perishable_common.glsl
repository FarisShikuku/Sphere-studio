// Perishable goods shared GLSL compute utilities
#ifndef PERISHABLE_COMMON_GLSL
#define PERISHABLE_COMMON_GLSL

float PerishHash(vec2 p) {
    p = fract(p * vec2(789.12, 567.34));
    p += dot(p, p + 12.34);
    return fract(p.x * p.y);
}

float PerishNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(PerishHash(i), PerishHash(i + vec2(1.0, 0.0)), f.x),
               mix(PerishHash(i + vec2(0.0, 1.0)), PerishHash(i + vec2(1.0, 1.0)), f.x), f.y);
}

// Organic skin cellular mottling
float OrganicSkinCells(vec2 uv) {
    float n1 = PerishNoise(uv * 32.0);
    float n2 = PerishNoise(uv * 96.0);
    return n1 * 0.7 + n2 * 0.3;
}

#endif
