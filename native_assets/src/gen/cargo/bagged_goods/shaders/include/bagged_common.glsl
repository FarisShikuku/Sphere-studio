// Bagged Goods shared GLSL compute utilities
#ifndef BAGGED_COMMON_GLSL
#define BAGGED_COMMON_GLSL

float BagHash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

float BagNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    float a = BagHash(i);
    float b = BagHash(i + vec2(1.0, 0.0));
    float c = BagHash(i + vec2(0.0, 1.0));
    float d = BagHash(i + vec2(1.0, 1.0));
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

float BagFbmFolds(vec2 uv, int octaves) {
    float val = 0.0;
    float amp = 0.5;
    vec2 p = uv;
    for (int i = 0; i < octaves; ++i) {
        val += amp * BagNoise(p);
        p *= 2.02;
        amp *= 0.5;
    }
    return val;
}

// Interlaced woven polypropylene thread weave pattern
float WovenFabricWeave(vec2 uv, float frequency) {
    vec2 grid = fract(uv * frequency);
    float warp = smoothstep(0.4, 0.5, grid.x) - smoothstep(0.5, 0.6, grid.x);
    float weft = smoothstep(0.4, 0.5, grid.y) - smoothstep(0.5, 0.6, grid.y);
    return sin(grid.x * 3.14159) * sin(grid.y * 3.14159);
}

#endif
