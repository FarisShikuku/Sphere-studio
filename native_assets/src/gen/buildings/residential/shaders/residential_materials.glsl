#ifndef RESIDENTIAL_MATERIALS_GLSL
#define RESIDENTIAL_MATERIALS_GLSL

vec3 evalResidentialSiding(vec2 uv, uint sidingStyle) {
    if (sidingStyle == 0u) {
        // Horizontal lap siding shadow lines
        float plank = smoothstep(0.0, 0.15, fract(uv.y * 16.0));
        return mix(vec3(0.72, 0.70, 0.66), vec3(0.90, 0.88, 0.84), plank);
    }
    if (sidingStyle == 1u) {
        // Vertical timber louver slats
        float slat = step(0.25, fract(uv.x * 24.0));
        return mix(vec3(0.12, 0.10, 0.08), vec3(0.58, 0.38, 0.22), slat);
    }
    return vec3(0.85, 0.83, 0.79);
}

#endif // RESIDENTIAL_MATERIALS_GLSL
