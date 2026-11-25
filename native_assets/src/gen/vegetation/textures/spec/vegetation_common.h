#pragma once
#include <cstdint>

namespace vegetation {
struct VegetationTextureFormat {
    static constexpr uint32_t kAlbedoWidth = 1024;
    static constexpr uint32_t kAlbedoHeight = 1024;
    static constexpr uint32_t kNormalChannels = 4; // RGBA8
};
}
