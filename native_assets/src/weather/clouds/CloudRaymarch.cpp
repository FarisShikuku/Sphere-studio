#include "CloudRaymarch.h"
#include <cmath>

namespace sphere::weather {

CloudRayResult CloudRaymarch::March(const Vec3& origin, const Vec3& dir, const CloudGen& generator, float time) const {
    CloudRayResult result{1.0f, {0.95f, 0.95f, 1.0f}};
    if (dir.y <= 0.01f) return result;

    float stepSize = 100.0f;
    float t = (generator.GetParams().baseAltitude - origin.y) / dir.y;
    if (t < 0.0f) t = 0.0f;

    float extinction = 0.0f;
    for (int i = 0; i < 32; ++i) {
        Vec3 pos{origin.x + dir.x * t, origin.y + dir.y * t, origin.z + dir.z * t};
        if (pos.y > generator.GetParams().topAltitude) break;

        float d = generator.SampleCloudDensity(pos, time);
        extinction += d * stepSize * 0.001f;
        t += stepSize;
    }

    result.transmittance = std::exp(-extinction);
    return result;
}

} // namespace sphere::weather
