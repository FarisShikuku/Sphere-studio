#ifndef DAMAGE_MASK_GLSL
#define DAMAGE_MASK_GLSL

float evalWallCrackAttenuation(float crackSample, float severity) {
    return clamp(1.0 - crackSample * severity * 0.85, 0.15, 1.0);
}

#endif // DAMAGE_MASK_GLSL
