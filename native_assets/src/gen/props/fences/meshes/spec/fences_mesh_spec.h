#pragma once

#include <cstdint>

namespace procedural::props {

struct FencesMeshSpec {
    uint32_t maxVerticesPerFence = 8192;
    uint32_t maxIndicesPerFence = 16384;
    float maxBaySpanMeters = 3.2f;
    float lodCullDistanceMeters = 85.0f;
};

} // namespace procedural::props
