#ifndef ROOFTOP_CLUTTER_GLSL
#define ROOFTOP_CLUTTER_GLSL

vec3 evalRooftopHardwareTint(uint kind) {
    if (kind == 0u) return vec3(0.62, 0.65, 0.68); // HVAC Galvanized steel
    if (kind == 1u) return vec3(0.55, 0.25, 0.18); // Brick chimney
    return vec3(0.75, 0.77, 0.80);                 // Mast/antenna
}

#endif // ROOFTOP_CLUTTER_GLSL
