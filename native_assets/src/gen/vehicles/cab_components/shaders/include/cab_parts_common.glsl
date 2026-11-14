// Shared GLSL definitions for Vulkan 1.3 procedural cab parts compute shaders
#ifndef CAB_PARTS_COMMON_GLSL
#define CAB_PARTS_COMMON_GLSL

struct CabPartPushConstants {
    vec4 paintColor;
    vec4 glassColor;
    uint partType;
    uint seed;
    float roughness;
    float metallic;
};

float hash12(vec2 p) {
    vec3 p3 = fract(vec3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

#endif
