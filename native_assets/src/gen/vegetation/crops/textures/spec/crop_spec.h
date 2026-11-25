#pragma once
#include <cstdint>

namespace vegetation {
struct CropTextureSpec {
    uint32_t stalkAtlasRes{1024};
    uint32_t earAtlasRes{1024};
    float drynessFactor{0.2f};
};
}
