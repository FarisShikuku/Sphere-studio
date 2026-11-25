#pragma once

#include <cstdint>

namespace procedural::props {

struct RuralPropTextureSpec {
    uint32_t diffuseResolution = 512;
    uint32_t normalResolution = 512;
    uint32_t ormResolution = 256;
    float weatheredWoodRoughness = 0.88f;
    float paintedWoodRoughness = 0.45f;
    float hayStrawRoughness = 0.95f;
    float galvanizedTroughRoughness = 0.52f;
    float galvanizedTroughMetallic = 0.78f;
    float waterSurfaceRoughness = 0.02f;
};

} // namespace procedural::props
