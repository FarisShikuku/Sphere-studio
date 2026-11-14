#ifndef TANKER_COMMON_GLSL
#define TANKER_COMMON_GLSL
float TankWeldSeam(vec2 uv, float rings) {
    float d = abs(fract(uv.x * rings) - 0.5);
    return 1.0 - smoothstep(0.0, 0.03, d);
}
#endif
