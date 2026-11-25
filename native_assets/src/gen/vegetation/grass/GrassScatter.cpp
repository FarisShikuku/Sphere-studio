// ============================================================================
// GrassScatter.cpp - Clump Scatter Implementation
// ============================================================================

#include "GrassScatter.h"
#include <cmath>

namespace vegetation {

std::vector<GrassBlade_C> GrassScatter::scatterClump(
    float centerWorldX,
    float centerWorldZ,
    float clumpRadiusMeters,
    int bladeCount,
    GrassSpecies_C species,
    uint64_t seed
) {
    FastRng rng(seed);
    std::vector<GrassBlade_C> blades;
    blades.reserve(bladeCount);

    for (int i = 0; i < bladeCount; ++i) {
        float r = clumpRadiusMeters * std::sqrt(rng.nextFloat01());
        float theta = rng.nextRange(0.0f, 6.2831853f);

        GrassBlade_C b;
        b.basePosition.x = centerWorldX + r * std::cos(theta);
        b.basePosition.y = 0.0f;
        b.basePosition.z = centerWorldZ + r * std::sin(theta);

        b.heightMeters = rng.nextRange(0.22f, 0.48f);
        b.widthMeters = rng.nextRange(0.015f, 0.035f);
        b.curvature = rng.nextRange(0.15f, 0.55f);
        b.headingRad = rng.nextRange(0.0f, 6.2831853f);
        b.tiltRad = rng.nextRange(0.05f, 0.35f);
        b.windBendFactor = rng.nextRange(0.6f, 1.0f);

        blades.push_back(b);
    }

    return blades;
}

} // namespace vegetation
