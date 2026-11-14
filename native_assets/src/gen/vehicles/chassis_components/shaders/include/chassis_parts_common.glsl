#ifndef CHASSIS_PARTS_COMMON_GLSL
#define CHASSIS_PARTS_COMMON_GLSL

struct ChassisPartPushConstants {
    vec4 alloyColor;
    vec4 steelColor;
    float wheelbase;
    float clearance;
    uint driveConfig;
    uint seed;
};

#endif
