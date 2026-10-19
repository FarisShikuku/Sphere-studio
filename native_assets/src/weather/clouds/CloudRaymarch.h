#pragma once
#include "CloudGen.h"

namespace sphere::weather {

struct CloudRayResult {
    float transmittance;
    Vec3 radiance;
};

class CloudRaymarch {
public:
    CloudRaymarch() = default;
    CloudRayResult March(const Vec3& origin, const Vec3& dir, const CloudGen& generator, float time) const;
};

} // namespace sphere::weather
