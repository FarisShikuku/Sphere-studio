#ifndef FENCE_COMMON_GLSL
#define FENCE_COMMON_GLSL

struct FencePushParams {
    float length;
    float height;
    float postSpacing;
    int style;
    int colorScheme;
    int hasAccentDetail;
};

struct FencePropVertex {
    vec4 position;
    vec4 normal;
    vec4 color;
};

uint computeFenceBayCount(float lengthMeters, float postSpacingMeters) {
    return uint(max(1, int(round(lengthMeters / max(postSpacingMeters, 0.5)))));
}

vec3 resolveFenceMetalTint(int colorScheme) {
    if (colorScheme == 1) return vec3(0.80, 0.84, 0.88); // Brushed silver
    if (colorScheme == 2) return vec3(0.47, 0.21, 0.06); // Architectural bronze
    if (colorScheme == 3) return vec3(0.92, 0.86, 0.72); // Champagne
    if (colorScheme == 4) return vec3(0.60, 0.20, 0.07); // Weathering Corten
    return vec3(0.07, 0.09, 0.12);                       // Obsidian black
}

#endif // FENCE_COMMON_GLSL
