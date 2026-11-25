#ifndef GATE_COMMON_GLSL
#define GATE_COMMON_GLSL

struct GatePushParams {
    float width;
    float height;
    float postDepth;
    int style;
    int colorScheme;
    int hasSmartHardware;
};

struct GatePropVertex {
    vec4 position;
    vec4 normal;
    vec4 color;
};

vec3 resolveGateMetalTint(int colorScheme) {
    if (colorScheme == 1) return vec3(0.80, 0.84, 0.88); // Anodized Silver
    if (colorScheme == 2) return vec3(0.47, 0.21, 0.06); // Architectural Bronze
    if (colorScheme == 3) return vec3(0.97, 0.98, 0.99); // Matte White
    if (colorScheme == 4) return vec3(0.60, 0.20, 0.07); // Weathering Corten
    return vec3(0.07, 0.09, 0.15);                       // Obsidian Charcoal
}

vec3 resolveGateWoodTint(int colorScheme) {
    if (colorScheme == 1) return vec3(0.47, 0.21, 0.06); // Dark Walnut
    if (colorScheme == 2) return vec3(0.82, 0.71, 0.55); // Blonde Oak
    if (colorScheme == 3) return vec3(0.20, 0.25, 0.33); // Shou Sugi Ban Slate
    return vec3(0.71, 0.33, 0.04);                       // Western Cedar
}

#endif // GATE_COMMON_GLSL
