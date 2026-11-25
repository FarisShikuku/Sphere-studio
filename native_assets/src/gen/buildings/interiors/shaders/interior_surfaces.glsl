#ifndef INTERIOR_SURFACES_GLSL
#define INTERIOR_SURFACES_GLSL

const float PI_INT = 3.14159265359;

vec3 evalInteriorFloorPattern(vec2 uv, uint roomType) {
    if (roomType == 0u) { // Living hardwood planks
        float plank = step(0.04, fract(uv.y * 12.0));
        return mix(vec3(0.42, 0.28, 0.16), vec3(0.64, 0.45, 0.28), plank);
    }
    // Office carpet / warehouse epoxy tile
    float grid = step(0.03, fract(uv.x * 6.0)) * step(0.03, fract(uv.y * 6.0));
    return mix(vec3(0.28, 0.31, 0.35), vec3(0.38, 0.41, 0.46), grid);
}

// Frostbite / Lagarde singularity-free windowed inverse-square bulb attenuation
float evalBulbWindowedInvSquare(float dist, float bulbRadius, float maxRange) {
    float safeD = max(dist, max(bulbRadius, 0.08));
    float r = dist / max(maxRange, 0.5);
    float win = clamp(1.0 - r * r * r * r, 0.0, 1.0);
    return (win * win) / (safeD * safeD);
}

// Planckian blackbody locus (Tanner Helland linear sRGB fit)
vec3 kelvinToLinearRgb(float kelvin) {
    float t = clamp(kelvin, 1500.0, 12000.0) / 100.0;
    float r = (t <= 66.0) ? 1.0 : clamp(329.6987274 * pow(t - 60.0, -0.1332047) / 255.0, 0.0, 1.0);
    float g = (t <= 66.0)
        ? clamp((99.4708025 * log(max(t, 1.0)) - 161.119568) / 255.0, 0.0, 1.0)
        : clamp(288.1221695 * pow(t - 60.0, -0.0755148) / 255.0, 0.0, 1.0);
    float b = (t >= 66.0) ? 1.0 : ((t <= 19.0) ? 0.0
        : clamp((138.5177312 * log(max(t - 10.0, 1.0)) - 305.0447927) / 255.0, 0.0, 1.0));
    return pow(vec3(r, g, b), vec3(2.2));
}

// Omnidirectional 3D world-space shadow bias matching SaschaWillems shadowmappingomni
float sampleOmniBulbShadow(samplerCube shadowCube, vec3 worldPos, vec3 bulbPos, vec3 N, float farPlane) {
    vec3 fragToLight = worldPos - bulbPos + N * 0.005;
    float currentDepth = length(fragToLight);
    float closestDepth = texture(shadowCube, fragToLight).r * farPlane;
    float bias = max(0.03 * (1.0 - dot(N, normalize(-fragToLight))), 0.008);
    return (currentDepth - bias <= closestDepth) ? 1.0 : 0.0;
}

#endif // INTERIOR_SURFACES_GLSL
