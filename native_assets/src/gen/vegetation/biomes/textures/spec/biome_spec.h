#pragma once
#include <cstdint>

namespace vegetation {
struct BiomeTextureSpec {
    uint32_t macroBiomeMapWidth{4096};
    uint32_t macroBiomeMapHeight{4096};
    uint32_t splatMapChannels{4};
    float blendTransitionBandMeters{25.0f};
};
}
