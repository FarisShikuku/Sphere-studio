#include "SunDisk.h"
#include <cmath>
#include <algorithm>

namespace sphere {

Vec3 SunDisk::RenderDisk(const Vec3& rayDir, const Vec3& sunDir, const Vec3& sunColor) const {
    if (!enabled) return Vec3{0.0f, 0.0f, 0.0f};

    float cosTheta = rayDir.x * sunDir.x + rayDir.y * sunDir.y + rayDir.z * sunDir.z;
    float diskCos = std::cos(angularDiameter);
    float coronaCos = std::cos(coronaRadius);

    if (cosTheta >= diskCos) {
        // Center solar disk with limb darkening
        float distFromCenter = std::sqrt(std::max(0.0f, 1.0f - cosTheta * cosTheta)) / std::sin(angularDiameter);
        float limbDarkening = 1.0f - 0.6f * (1.0f - std::sqrt(std::max(0.0f, 1.0f - distFromCenter * distFromCenter)));
        float lum = intensity * limbDarkening;
        return Vec3{sunColor.x * lum, sunColor.y * lum, sunColor.z * lum};
    } else if (cosTheta >= coronaCos) {
        // Atmospheric corona bloom
        float factor = (cosTheta - coronaCos) / (diskCos - coronaCos);
        float bloom = std::pow(factor, 4.0f) * (intensity * 0.15f);
        return Vec3{sunColor.x * bloom, sunColor.y * bloom * 0.9f, sunColor.z * bloom * 0.7f};
    }

    return Vec3{0.0f, 0.0f, 0.0f};
}

} // namespace sphere
