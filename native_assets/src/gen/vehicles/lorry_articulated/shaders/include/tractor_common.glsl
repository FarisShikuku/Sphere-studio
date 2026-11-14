#ifndef TRACTOR_COMMON_GLSL
#define TRACTOR_COMMON_GLSL

struct TractorPushConstants {
    vec4 cabColor;
    vec4 accentColor;
    float wheelbase;
    float fifthWheelOffset;
    uint variantId;
    uint seed;
};

#endif
