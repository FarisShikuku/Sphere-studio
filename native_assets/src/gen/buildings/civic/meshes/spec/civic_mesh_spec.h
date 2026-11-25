#pragma once

#include <cstdint>

namespace procedural::buildings {

struct CivicMeshSpec {
    uint32_t maxVertices = 65536;
    float colonnadeSpacingMeters = 4.5f;
    float domeSegments = 32.0f;
    float canopyOverhangMeters = 2.8f;
};

} // namespace procedural::buildings
