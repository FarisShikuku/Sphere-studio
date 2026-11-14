#ifndef GEAR_COMMON_GLSL
#define GEAR_COMMON_GLSL

float GearHash(vec2 p) {
    p = fract(p * vec2(234.34, 567.21));
    p += dot(p, p + 34.45);
    return fract(p.x * p.y);
}

// Heavy commercial highway tire tread block pattern
float TireTreadHeight(vec2 uv) {
    float rib = smoothstep(0.18, 0.24, abs(fract(uv.x * 5.0) - 0.5));
    float lug = smoothstep(0.15, 0.22, abs(fract(uv.y * 28.0 + 0.15 * sin(uv.x * 12.0)) - 0.5));
    return rib * lug;
}

// Radial ABS exciter tone-ring teeth
float AbsToneRing(vec2 centeredUv) {
    float r = length(centeredUv);
    float ang = atan(centeredUv.y, centeredUv.x);
    float ringBand = smoothstep(0.28, 0.30, r) * (1.0 - smoothstep(0.36, 0.38, r));
    return ringBand * step(0.5, fract(ang * 48.0 / 6.2831853));
}

#endif
