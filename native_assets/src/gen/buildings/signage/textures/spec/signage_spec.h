#pragma once

#include <cstdint>

namespace procedural::buildings {

struct SignageTextureSpec {
    uint32_t glowResolution = 512;
    uint32_t billboardResolution = 1024;
    float neonTubeEmissionStrength = 6.0f;
    float billboardBackingRoughness = 0.50f;
};

} // namespace procedural::buildings
