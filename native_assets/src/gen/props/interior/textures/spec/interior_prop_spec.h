#pragma once

#include <cstdint>

namespace procedural::props {

struct InteriorPropTextureSpec {
    uint32_t diffuseResolution = 512;
    uint32_t normalResolution = 512;
    uint32_t ormResolution = 256;
    float polishedWoodRoughness = 0.25f;
    float matteWoodRoughness = 0.58f;
    float fabricUpholsteryRoughness = 0.85f;
    float leatherRoughness = 0.42f;
    float chromeMetallic = 0.95f;
    float chromeRoughness = 0.10f;
    float deskMatRoughness = 0.65f;
};

} // namespace procedural::props
