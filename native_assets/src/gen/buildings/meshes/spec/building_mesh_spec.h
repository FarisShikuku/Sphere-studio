#pragma once

#include <cstdint>

namespace procedural::buildings {

struct BuildingMeshSpec {
    uint32_t maxVerticesPerBuilding = 65536;
    uint32_t maxIndicesPerBuilding = 131072;
    uint32_t maxAttachedSockets = 64;
    float minFloorHeightMeters = 2.6f;
    float maxFloorHeightMeters = 5.5f;
    bool enableInstancedCityBlocks = true;
};

} // namespace procedural::buildings
