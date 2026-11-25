#pragma once

#include <cstdint>

namespace procedural::props {

struct GatesMeshSpec {
    uint32_t maxVerticesPerGate = 12288;
    uint32_t maxIndicesPerGate = 24576;
    float minOpeningWidthMeters = 1.4f;
    float maxOpeningWidthMeters = 10.0f;
    float lodCullDistanceMeters = 110.0f;
};

} // namespace procedural::props
