#pragma once

#include <cstdint>

namespace procedural::buildings {

struct BuildingCommonTextureSpec {
    uint32_t albedoResolution = 1024;
    uint32_t normalResolution = 1024;
    uint32_t ormResolution = 512;
    float concreteRoughness = 0.85f;
    float brickRoughness = 0.82f;
    float glassRoughness = 0.04f;
    float glassIor = 1.52f;
    float metalFrameMetallic = 0.90f;
    float weatheringDirtFactor = 0.20f;
};

} // namespace procedural::buildings
