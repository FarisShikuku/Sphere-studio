#pragma once

#include <cstdint>

namespace procedural::buildings {

struct SignageMeshSpec {
    uint32_t maxVertices = 8192;
    float neonTubeRadiusMeters = 0.025f;
    float billboardFrameDepthMeters = 0.35f;
};

} // namespace procedural::buildings
