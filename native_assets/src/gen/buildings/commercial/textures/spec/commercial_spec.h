#pragma once

#include <cstdint>

namespace procedural::buildings {

struct CommercialTextureSpec {
    uint32_t curtainWallResolution = 1024;
    uint32_t spandrelResolution = 512;
    float curtainGlassRoughness = 0.03f;
    float anodizedMullionMetallic = 0.92f;
    float anodizedMullionRoughness = 0.28f;
    float interiorWarmGlowIntensity = 1.6f;
};

} // namespace procedural::buildings
