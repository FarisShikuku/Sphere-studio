#pragma once

#include <cstdint>

namespace procedural::buildings {

struct BuildingLodTier {
    uint32_t tierIndex = 0;
    float maxDistanceMeters = 60.0f;
    bool enableInteriorShell = true;
    bool enableWindowMullions = true;
    bool enableRooftopProps = true;
};

constexpr BuildingLodTier kBuildingLodTiers[3] = {
    {0, 60.0f, true, true, true},
    {1, 180.0f, false, true, false},
    {2, 600.0f, false, false, false}
};

} // namespace procedural::buildings
