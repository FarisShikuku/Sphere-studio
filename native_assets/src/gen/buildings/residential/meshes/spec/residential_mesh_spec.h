#pragma once

#include <cstdint>

namespace procedural::buildings {

struct ResidentialMeshSpec {
    uint32_t maxVertices = 32768;
    float defaultFloorHeight = 3.0f;
    float defaultBalconyDepth = 1.25f;
    float roofOverhangMeters = 0.55f;
};

} // namespace procedural::buildings
