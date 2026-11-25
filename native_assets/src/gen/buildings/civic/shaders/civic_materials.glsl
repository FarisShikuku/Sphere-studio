#ifndef CIVIC_MATERIALS_GLSL
#define CIVIC_MATERIALS_GLSL

vec3 evalCivicLimestoneCourse(vec2 uv, float courses) {
    float line = smoothstep(0.0, 0.04, fract(uv.y * courses));
    return mix(vec3(0.68, 0.65, 0.60), vec3(0.86, 0.83, 0.77), line);
}

#endif // CIVIC_MATERIALS_GLSL
