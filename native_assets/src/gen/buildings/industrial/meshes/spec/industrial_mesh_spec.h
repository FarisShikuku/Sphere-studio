#pragma once

#include <cstdint>

namespace procedural::buildings {

struct IndustrialMeshSpec {
    uint32_t maxVertices = 32768;
    float warehouseClearHeight = 6.5f;
    float trussBaySpacing = 6.0f;
    float rollDoorWidth = 4.2f;
};

} // namespace procedural::buildings
