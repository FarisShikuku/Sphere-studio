#pragma once
#include <cstdint>

namespace sphere::weather {

struct WeatherCommonTextureSpec {
    static constexpr uint32_t SEED_MAGIC = 0x57544852; // "WTHR"
    static constexpr uint32_t VERSION = 1;
    static constexpr uint32_t ATLAS_WIDTH = 1024;
    static constexpr uint32_t ATLAS_HEIGHT = 1024;
    static constexpr uint32_t NOISE_OCTAVES = 4;
};

} // namespace sphere::weather
