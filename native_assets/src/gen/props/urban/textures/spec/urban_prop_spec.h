#pragma once

#include <cstdint>

namespace procedural::props {

struct UrbanPropTextureSpec {
    uint32_t diffuseResolution = 512;
    uint32_t normalResolution = 512;
    uint32_t ormResolution = 256;
    float fireEngineRedRoughness = 0.28f;
    float castIronHydrantMetallic = 0.70f;
    float postalBoxGloss = 0.85f;
    float busShelterGlassRoughness = 0.05f;
    float busShelterGlassTransmission = 0.94f;
    float powderCoatedFrameRoughness = 0.40f;
};

} // namespace procedural::props
