#pragma once

#include <cstdint>

namespace procedural::buildings {

struct BuildingPropMeshSpec {
    uint32_t maxAttachedProps = 24;
    float acUnitHeightMeters = 0.95f;
    float chimneyFlueWidthMeters = 0.65f;
    float antennaMastHeightMeters = 3.2f;
};

} // namespace procedural::buildings
