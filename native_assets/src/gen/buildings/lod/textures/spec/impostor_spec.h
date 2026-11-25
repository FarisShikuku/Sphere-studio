#pragma once

#include <cstdint>

namespace procedural::buildings {

struct ImpostorTextureSpec {
    uint32_t atlasResolution = 2048;
    uint32_t octahedralSlices = 8;
    float impostorSwitchDistanceMeters = 180.0f;
};

} // namespace procedural::buildings
