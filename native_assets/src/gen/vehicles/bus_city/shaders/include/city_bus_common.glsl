#ifndef CITY_BUS_COMMON_GLSL
#define CITY_BUS_COMMON_GLSL

struct CityBusPushConstants {
    vec4 liveryColor;
    vec4 accentColor;
    float floorHeight;
    float doorOpenRatio;
    uint variantId;
    uint passengerCap;
};

#endif
