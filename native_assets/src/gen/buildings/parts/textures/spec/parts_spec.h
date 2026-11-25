#pragma once

#include <cstdint>

namespace procedural::buildings {

struct PartsTextureSpec {
    uint32_t wallResolution = 512;
    uint32_t roofResolution = 512;
    float frameMetalRoughness = 0.28f;
    float doorTimberRoughness = 0.62f;
    float shingleRoughness = 0.85f;
};

} // namespace procedural::buildings
