#pragma once
#include <cstdint>

namespace vegetation {
struct DebrisTextureSpec {
    uint32_t debrisAtlasRes{2048};
    uint32_t deadLeafVariants{8};
    uint32_t twigVariants{6};
    float humusDarkness{0.35f};
};
}
