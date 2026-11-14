#ifndef RIGID_LORRY_COMMON_GLSL
#define RIGID_LORRY_COMMON_GLSL

struct RigidLorryPushConstants {
    vec4 cabColor;
    vec4 bodyColor;
    float wheelbase;
    float cabHeight;
    float actionRatio;
    uint variantId;
};

#endif
