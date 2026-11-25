#pragma once

#include <cstdint>

namespace procedural::buildings {

struct InteriorMeshSpec {
    uint32_t maxVertices = 16384;
    float partitionWallThickness = 0.14f;
    float ceilingClearanceMeters = 2.75f;
};

} // namespace procedural::buildings
