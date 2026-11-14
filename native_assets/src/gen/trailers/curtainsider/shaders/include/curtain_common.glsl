#ifndef CURTAIN_COMMON_GLSL
#define CURTAIN_COMMON_GLSL
float CurtainWave(vec2 uv, float speed, float time) {
    float strapX = abs(fract(uv.x * 10.0) - 0.5);
    float tensionPinch = smoothstep(0.0, 0.12, strapX);
    float billow = sin(uv.x * 31.4159 + time * speed) * cos(uv.y * 6.28318);
    return billow * (0.015 + 0.025 * tensionPinch);
}
#endif
