#ifndef PARTS_COMMON_GLSL
#define PARTS_COMMON_GLSL

float evalRoofSlopeHeight(float distFromCenter, float halfSpan, float roofHeight, uint roofType) {
    float t = clamp(distFromCenter / max(halfSpan, 0.001), 0.0, 1.0);
    if (roofType == 4u) { // Dome
        return roofHeight * sqrt(max(0.0, 1.0 - t * t));
    }
    return roofHeight * (1.0 - t);
}

#endif // PARTS_COMMON_GLSL
