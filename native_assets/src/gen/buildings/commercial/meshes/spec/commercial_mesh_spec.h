#pragma once

#include <cstdint>

namespace procedural::buildings {

struct CommercialMeshSpec {
    uint32_t maxVertices = 49152;
    float defaultFloorHeight = 3.6f;
    float mullionWidthMeters = 0.08f;
    float finProjectionMeters = 0.35f;
};

} // namespace procedural::buildings
