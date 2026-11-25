#pragma once

#include <cstdint>

namespace procedural::buildings {

struct InteriorTextureSpec {
    uint32_t floorplanResolution = 512;
    float hardwoodRoughness = 0.42f;
    float drywallRoughness = 0.88f;
    float epoxyFloorRoughness = 0.20f;
};

} // namespace procedural::buildings
