#ifndef INDUSTRIAL_PBR_GLSL
#define INDUSTRIAL_PBR_GLSL

vec3 evalCorrugatedSteelAlbedo(vec2 uv, float pitch, vec3 baseCol) {
    float wave = 0.5 + 0.5 * sin(uv.x * pitch * 6.2831853);
    return baseCol * (0.78 + 0.22 * wave);
}

#endif // INDUSTRIAL_PBR_GLSL
