#pragma once
// ============================================================================
// DebrisGen.h - Forest Detritus Scatter Generator in C++17
// ============================================================================

#include "DebrisParams.h"
#include "../common/VegetationMath.h"
#include <vector>

namespace vegetation {

struct DebrisItem {
    VegVec3_C position;
    float yawRad;
    float scale;
    DebrisType type;
    VegColor_C tint;
};

class DebrisGen {
public:
    static std::vector<DebrisItem> scatterDebrisPatch(
        float minX, float maxX,
        float minZ, float maxZ,
        const DebrisScatterConfig& cfg,
        uint64_t seed
    );
};

} // namespace vegetation
