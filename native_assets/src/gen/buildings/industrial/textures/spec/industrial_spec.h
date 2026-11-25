#pragma once

#include <cstdint>

namespace procedural::buildings {

struct IndustrialTextureSpec {
    uint32_t claddingResolution = 1024;
    uint32_t rollDoorResolution = 512;
    float corrugatedSteelRoughness = 0.48f;
    float corrugatedSteelMetallic = 0.85f;
    float epoxyFloorRoughness = 0.25f;
};

} // namespace procedural::buildings
