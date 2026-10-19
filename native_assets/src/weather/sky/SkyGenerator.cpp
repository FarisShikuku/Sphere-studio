#include "SkyGenerator.h"
#include <algorithm>

namespace sphere::weather {

Vec3 SkyGenerator::ComputeSkyRadiance(const Vec3& viewDir, const Vec3& sunDir, float stormFactor) const {
    float elevation = std::max(0.0f, viewDir.y);
    Vec3 skyBase{0.4f * (1.0f - stormFactor), 0.5f * (1.0f - stormFactor), 0.7f * (1.0f - stormFactor)};
    Vec3 horizonBase{0.7f * (1.0f - stormFactor), 0.75f * (1.0f - stormFactor), 0.8f * (1.0f - stormFactor)};

    return Vec3{
        horizonBase.x + (skyBase.x - horizonBase.x) * elevation,
        horizonBase.y + (skyBase.y - horizonBase.y) * elevation,
        horizonBase.z + (skyBase.z - horizonBase.z) * elevation
    };
}

} // namespace sphere::weather
