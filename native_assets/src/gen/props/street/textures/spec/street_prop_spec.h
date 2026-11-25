#pragma once

#include <cstdint>

namespace procedural::props {

struct StreetPropTextureSpec {
    uint32_t diffuseResolution = 512;
    uint32_t normalResolution = 512;
    uint32_t ormResolution = 256;
    float paintedMetalRoughness = 0.35f;
    float galvanizedPoleRoughness = 0.55f;
    float benchWoodRoughness = 0.72f;
    float castIronMetallic = 0.82f;
    float trafficLightLensEmission = 3.5f;
};

} // namespace procedural::props
