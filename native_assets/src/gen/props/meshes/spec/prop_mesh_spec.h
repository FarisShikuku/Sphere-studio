#pragma once

#include <cstdint>

namespace procedural::props {

struct PropMeshSpec {
    uint32_t maxVerticesPerProp = 4096;
    uint32_t maxIndicesPerProp = 8192;
    float globalLODDistanceMeters = 50.0f;
    bool enableInstancedRendering = true;
};

} // namespace procedural::props
