#pragma once

#include <cstdint>

namespace procedural::props {

struct PropCommonTextureSpec {
    uint32_t diffuseResolution = 512;
    uint32_t normalResolution = 512;
    uint32_t ormResolution = 256;
    float globalWeatheringIntensity = 0.25f;
    float dustAccumulationFactor = 0.15f;
};

} // namespace procedural::props
