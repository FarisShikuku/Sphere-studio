// Liquid and Bulk carriers shared GLSL compute utilities
#ifndef LIQUID_COMMON_GLSL
#define LIQUID_COMMON_GLSL

float LiquidHash(vec2 p) {
    p = fract(p * vec2(345.67, 123.89));
    p += dot(p, p + 67.89);
    return fract(p.x * p.y);
}

float LiquidNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(LiquidHash(i), LiquidHash(i + vec2(1.0, 0.0)), f.x),
               mix(LiquidHash(i + vec2(0.0, 1.0)), LiquidHash(i + vec2(1.0, 1.0)), f.x), f.y);
}

// Procedural rust and paint chipping noise
float RustFlaking(vec2 uv) {
    float n1 = LiquidNoise(uv * 16.0);
    float n2 = LiquidNoise(uv * 64.0);
    return smoothstep(0.55, 0.72, n1 * 0.7 + n2 * 0.3);
}

#endif
