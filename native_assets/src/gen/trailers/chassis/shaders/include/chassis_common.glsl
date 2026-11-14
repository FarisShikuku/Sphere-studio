#ifndef CHASSIS_COMMON_GLSL
#define CHASSIS_COMMON_GLSL
float ChassisPanelPattern(vec2 uv) {
    float seamX = smoothstep(0.0, 0.02, abs(fract(uv.x * 8.0) - 0.5));
    float seamY = smoothstep(0.0, 0.02, abs(fract(uv.y * 4.0) - 0.5));
    return seamX * seamY;
}
#endif
