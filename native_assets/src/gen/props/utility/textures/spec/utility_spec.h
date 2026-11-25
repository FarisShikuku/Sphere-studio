#pragma once

#include <cstdint>

namespace procedural::props {

struct UtilityTextureSpec {
    uint32_t diffuseResolution = 512;
    uint32_t normalResolution = 512;
    uint32_t roughnessMetallicResolution = 256;
    float woodGrainRoughness = 0.78f;
    float concretePorosity = 0.65f;
    float galvanizedMetallic = 0.88f;
    float ceramicSmoothness = 0.05f;
    float glassTransmission = 0.92f;
};

} // namespace procedural::props
