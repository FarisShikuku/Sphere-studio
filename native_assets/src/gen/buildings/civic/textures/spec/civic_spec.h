#pragma once

#include <cstdint>

namespace procedural::buildings {

struct CivicTextureSpec {
    uint32_t facadeResolution = 1024;
    uint32_t domeAtlasResolution = 512;
    float limestoneRoughness = 0.78f;
    float copperPatinaRoughness = 0.55f;
    float stainedGlassTransmission = 0.65f;
};

} // namespace procedural::buildings
