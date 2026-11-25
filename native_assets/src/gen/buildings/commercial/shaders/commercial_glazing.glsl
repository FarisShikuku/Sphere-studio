#ifndef COMMERCIAL_GLAZING_GLSL
#define COMMERCIAL_GLAZING_GLSL

vec3 evalCommercialSpandrelBand(vec2 uv, float spandrelRatio, vec3 glassCol, vec3 aluminumCol) {
    float f = fract(uv.y);
    float isMullion = step(f, 0.03) + step(0.97, f);
    if (isMullion > 0.5) return aluminumCol;
    return (f < spandrelRatio) ? aluminumCol * 0.65 : glassCol;
}

vec4 evalCommercialRefractiveGlass(
    vec2 uv,
    float spandrelRatio,
    vec3 aluminumCol,
    vec3 worldPos,
    vec3 N,
    vec3 V,
    vec3 sunDir,
    uint glassType,
    float windowIor
) {
    float f = fract(uv.y);
    float isMullion = step(f, 0.03) + step(0.97, f);
    if (isMullion > 0.5) return vec4(aluminumCol, 1.0);
    if (f < spandrelRatio) return vec4(aluminumCol * 0.45, 1.0);
    float alpha = 1.0;
    vec3 glassCol = evaluateRefractiveArchitecturalGlassPBR(
        glassType, worldPos, N, V, sunDir, uv, windowIor, alpha
    );
    return vec4(glassCol, alpha);
}

#endif // COMMERCIAL_GLAZING_GLSL
