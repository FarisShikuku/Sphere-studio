#pragma once

#include <cstdint>

namespace procedural::props {

struct FencesTextureSpec {
    uint32_t diffuseResolution = 512;
    uint32_t normalResolution = 512;
    uint32_t ormResolution = 256;
    float temperedGlassRoughness = 0.03f;
    float brushedSteelRoughness = 0.28f;
    float brushedSteelMetallic = 0.92f;
    float cortenRustRoughness = 0.78f;
    float precastConcreteRoughness = 0.85f;
    float cedarTimberRoughness = 0.72f;
    float foliageRoughness = 0.68f;
};

} // namespace procedural::props
