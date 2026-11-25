#pragma once
#include <cstdint>

namespace vegetation {
struct FlowerTextureSpec {
    uint32_t petalAtlasRes{1024};
    uint32_t discAtlasRes{512};
    float petalTranslucency{0.6f};
};
}
