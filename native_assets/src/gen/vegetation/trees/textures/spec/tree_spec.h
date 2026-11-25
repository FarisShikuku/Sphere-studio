#pragma once
#include <cstdint>

namespace vegetation {
struct TreeTextureAtlasSpec {
    uint32_t barkAtlasWidth{2048};
    uint32_t barkAtlasHeight{2048};
    uint32_t leafAtlasWidth{4096};
    uint32_t leafAtlasHeight{4096};
    uint32_t format{0x8058}; // GL_RGBA8
    float normalMapStrength{1.25f};
    float roughnessDefault{0.82f};
    bool generateMipmaps{true};
};
}
