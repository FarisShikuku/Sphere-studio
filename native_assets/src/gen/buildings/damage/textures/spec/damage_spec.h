#pragma once

#include <cstdint>

namespace procedural::buildings {

struct DamageTextureSpec {
    uint32_t crackMaskResolution = 512;
    float charredSootRoughness = 0.96f;
    float exposedRebarMetallic = 0.80f;
};

} // namespace procedural::buildings
