#pragma once
#include <cstdint>

namespace vegetation {
struct GrassTextureSpec {
    uint32_t atlasWidth{1024};
    uint32_t atlasHeight{2048};
    uint32_t bladeProfiles{16};
    float translucencyFactor{0.45f};
};
}
