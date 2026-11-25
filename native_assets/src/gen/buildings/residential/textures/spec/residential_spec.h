#pragma once

#include <cstdint>

namespace procedural::buildings {

struct ResidentialTextureSpec {
    uint32_t facadeResolution = 1024;
    uint32_t roofTileResolution = 512;
    float stuccoRoughness = 0.88f;
    float brickRoughness = 0.82f;
    float timberLouverRoughness = 0.65f;
    float slateTileRoughness = 0.70f;
};

} // namespace procedural::buildings
