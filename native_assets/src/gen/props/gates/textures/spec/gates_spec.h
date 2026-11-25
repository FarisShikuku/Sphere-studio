#pragma once

#include <cstdint>

namespace procedural::props {

struct GatesTextureSpec {
    uint32_t diffuseResolution = 512;
    uint32_t normalResolution = 512;
    uint32_t ormResolution = 256;
    float powderCoatedMetalRoughness = 0.34f;
    float anodizedMetalMetallic = 0.88f;
    float temperedGlassRoughness = 0.03f;
    float hardwoodSlatRoughness = 0.65f;
    float gabionStoneRoughness = 0.90f;
    float ledBeaconEmission = 4.2f;
};

} // namespace procedural::props
