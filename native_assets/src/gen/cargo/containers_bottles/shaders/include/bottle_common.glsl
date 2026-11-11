// Bottle and container shared GLSL compute utilities
#ifndef BOTTLE_COMMON_GLSL
#define BOTTLE_COMMON_GLSL

float BottleHash(vec2 p) {
    p = fract(p * vec2(567.89, 345.12));
    p += dot(p, p + 89.12);
    return fract(p.x * p.y);
}

float BottleNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(BottleHash(i), BottleHash(i + vec2(1.0, 0.0)), f.x),
               mix(BottleHash(i + vec2(0.0, 1.0)), BottleHash(i + vec2(1.0, 1.0)), f.x), f.y);
}

// Vertical parting line / mold seam along opposite sides (u=0.25 and u=0.75)
float GlassMoldSeam(float u) {
    float seam1 = exp(-pow((u - 0.25) * 200.0, 2.0));
    float seam2 = exp(-pow((u - 0.75) * 200.0, 2.0));
    return seam1 + seam2;
}

#endif
