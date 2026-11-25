#pragma once

#include <cstdint>

namespace procedural::props {

struct CommunicationTextureSpec {
    uint32_t resolution = 512;
    float whiteCoatingRoughness = 0.35f;
    float redCoatingRoughness = 0.35f;
    float galvanizedMetallic = 0.90f;
    float fiberglassRoughness = 0.25f;
};

} // namespace procedural::props
