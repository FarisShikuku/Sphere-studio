#pragma once

#include <cstdint>

namespace procedural::buildings {

struct DamageMeshSpec {
    uint32_t maxRubbleChunks = 128;
    float maxCrackDisplacementMeters = 0.18f;
};

} // namespace procedural::buildings
