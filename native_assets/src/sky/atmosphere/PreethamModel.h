#pragma once
#include "scene/Transform.h"

namespace sphere {

/**
 * Preetham Atmospheric Scattering Model (kentril0/WaterSurfaceRendering style).
 * Models daylight spectrum, Perez luminance formulation, and color grading.
 */
class PreethamModel {
public:
    float turbidity = 2.5f;

    void SetTurbidity(float t) { turbidity = t; }
    Vec3 CalculateZenithColor(float sunTheta) const;
    Vec3 EvaluatePerez(float theta, float gamma) const;
};

} // namespace sphere
