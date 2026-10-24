#pragma once
#include "CloudGen.h"

namespace sphere::weather {

struct CloudRayResult {
    float transmittance = 1.0f; // [0..1]
    Vec3 radiance{0.0f, 0.0f, 0.0f};
    float cloudAlpha = 0.0f;    // [0..1]
    float depthMeters = 0.0f;
};

class CloudRaymarch {
public:
    CloudRaymarch() = default;

    // Primary volumetric raymarcher through planetary cloud layer
    CloudRayResult March(
        const Vec3& origin,
        const Vec3& dir,
        const Vec3& sunDir,
        const Vec3& sunColor,
        const CloudGen& generator,
        float time
    ) const;

    // Secondary cone march towards sunlight to calculate self-shadowing and silver lining
    float RaymarchToLight(
        const Vec3& pos,
        const Vec3& sunDir,
        const CloudGen& generator,
        float time,
        float stepSize
    ) const;

    // Intersect spherical atmospheric cloud bounds (inner & outer shells)
    bool IntersectAtmosphere(
        const Vec3& ro,
        const Vec3& rd,
        float innerRadius,
        float outerRadius,
        float earthRadius,
        float& outNear,
        float& outFar
    ) const;
};

} // namespace sphere::weather
