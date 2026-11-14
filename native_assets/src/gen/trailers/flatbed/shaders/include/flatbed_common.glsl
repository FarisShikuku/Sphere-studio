#ifndef FLATBED_COMMON_GLSL
#define FLATBED_COMMON_GLSL
float WoodDeckPlankGrain(vec2 uv) {
    float plank = floor(uv.y * 12.0);
    float seam = smoothstep(0.0, 0.04, fract(uv.y * 12.0)) * (1.0 - smoothstep(0.96, 1.0, fract(uv.y * 12.0)));
    float grain = sin(uv.x * 95.0 + sin(uv.y * 24.0 + plank * 3.1) * 2.0) * 0.5 + 0.5;
    return mix(0.35, 0.7 + 0.3 * grain, seam);
}
#endif
