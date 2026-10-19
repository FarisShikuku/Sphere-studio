#pragma once
#include <cstdint>

namespace sphere::weather {

struct WeatherSkySpec {
    static constexpr uint32_t LUT_WIDTH = 64;
    static constexpr uint32_t LUT_HEIGHT = 64;
};

} // namespace sphere::weather
