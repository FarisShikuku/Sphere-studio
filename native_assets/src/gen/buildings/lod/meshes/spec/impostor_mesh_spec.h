#pragma once

#include <cstdint>

namespace procedural::buildings {

struct ImpostorMeshSpec {
    uint32_t billboardQuadVertices = 4;
    uint32_t maxCityBlockInstances = 4096;
};

} // namespace procedural::buildings
