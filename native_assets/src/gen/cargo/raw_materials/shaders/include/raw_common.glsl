// Raw materials shared GLSL compute utilities
#ifndef RAW_COMMON_GLSL
#define RAW_COMMON_GLSL

float RawHash(vec2 p) {
    p = fract(p * vec2(456.78, 234.56));
    p += dot(p, p + 78.91);
    return fract(p.x * p.y);
}

float RawNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(RawHash(i), RawHash(i + vec2(1.0, 0.0)), f.x),
               mix(RawHash(i + vec2(0.0, 1.0)), RawHash(i + vec2(1.0, 1.0)), f.x), f.y);
}

// Tree bark vertical furrow noise
float TreeBarkFurrows(vec2 uv) {
    float n1 = RawNoise(vec2(uv.x * 24.0, uv.y * 3.0));
    float n2 = RawNoise(vec2(uv.x * 48.0, uv.y * 6.0));
    return abs(n1 * 0.7 + n2 * 0.3 - 0.5) * 2.0;
}

#endif
