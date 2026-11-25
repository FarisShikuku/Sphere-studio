#pragma once

#include <cstdint>

namespace procedural::props {

struct SignageStreetTextureSpec {
    uint32_t resolution = 512;
    float retroreflectiveSheetingRoughness = 0.15f;
    float retroreflectiveGain = 4.5f;
    float weatheredStoneRoughness = 0.85f;
    float galvanizedSteelMetallic = 0.88f;
};

} // namespace procedural::props
