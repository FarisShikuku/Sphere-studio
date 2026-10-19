#pragma once
#include <cstdint>

namespace sphere::weather {

struct RainSpec {
    static constexpr uint32_t ATLAS_STREAKS = 16;
    static constexpr uint32_t SPLASH_RINGS = 3;
};

} // namespace sphere::weather
