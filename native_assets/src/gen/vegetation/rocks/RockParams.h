#pragma once
// ============================================================================
// RockParams.h - Geological Classification & Weathering Parameters in C++17
// ============================================================================

#include <cstdint>

namespace vegetation {

enum class RockGeology : uint32_t {
    GRANITE_BOULDER = 0,
    SANDSTONE_SLAB = 1,
    BASALT_HEXAGON = 2,
    LIMESTONE_CLIFF = 3,
    RIVER_COBBLE = 4
};

struct RockSynthesisParams {
    RockGeology geology{RockGeology::GRANITE_BOULDER};
    float scaleMeters{2.5f};
    float angularity{0.65f};
    float erosionFactor{0.35f};
    int subdivisionLevel{2};
    float mossCoverage{0.25f};
    float colorR{0.55f}, colorG{0.52f}, colorB{0.48f};
};

RockSynthesisParams getPredefinedRockParams(RockGeology geology);

} // namespace vegetation
