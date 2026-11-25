#pragma once

#include <cstdint>

namespace procedural::props {

struct IndustrialPropTextureSpec {
    uint32_t diffuseResolution = 512;
    uint32_t normalResolution = 512;
    uint32_t ormResolution = 256;
    float paintedSteelMetallic = 0.85f;
    float steelRoughness = 0.38f;
    float untreatedWoodRoughness = 0.75f;
    float rustPittingDepth = 0.04f;
    float stenciledPaintOpacity = 0.95f;
};

} // namespace procedural::props
