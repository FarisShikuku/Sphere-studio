#pragma once
#include "scene/Transform.h"

namespace sphere {

/**
 * Sun Disk & Solar Flare Generator (kentril0/WaterSurfaceRendering style).
 * Simulates direct solar disk luminance, limb darkening, and atmospheric corona.
 */
class SunDisk {
public:
    float angularDiameter = 0.045f;
    float coronaRadius = 0.22f;
    float intensity = 25.0f;
    bool enabled = true;

    Vec3 RenderDisk(const Vec3& rayDir, const Vec3& sunDir, const Vec3& sunColor) const;
};

} // namespace sphere
