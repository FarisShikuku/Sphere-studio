#ifndef GATE_FINISH_GLSL
#define GATE_FINISH_GLSL

vec3 evalGateFinishColor(vec2 uv, uint finishType) {
    if (finishType == 0u) { // Anodized brushed aluminum
        float brush = 0.94 + 0.06 * sin(uv.x * 160.0);
        return vec3(0.76, 0.80, 0.85) * brush;
    }
    if (finishType == 1u) { // Architectural bronze
        return vec3(0.45, 0.22, 0.08);
    }
    // Solar PV dark blue cell texture
    float grid = step(0.04, fract(uv.x * 8.0)) * step(0.04, fract(uv.y * 8.0));
    return mix(vec3(0.84, 0.87, 0.92), vec3(0.11, 0.22, 0.54), grid);
}

#endif // GATE_FINISH_GLSL
