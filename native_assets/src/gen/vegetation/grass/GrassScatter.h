#pragma once
// ============================================================================
// GrassScatter.h - Poisson-Disk & Jittered Clump Scatterer in C++17
// ============================================================================

#include "GrassTypes.h"
#include <vector>

namespace vegetation {

class GrassScatter {
public:
    static std::vector<GrassBlade_C> scatterClump(
        float centerWorldX,
        float centerWorldZ,
        float clumpRadiusMeters,
        int bladeCount,
        GrassSpecies_C species,
        uint64_t seed
    );
};

} // namespace vegetation
