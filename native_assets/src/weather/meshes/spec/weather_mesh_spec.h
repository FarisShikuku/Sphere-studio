#pragma once
#include <cstdint>

namespace sphere::weather {

struct WeatherMeshSpec {
    static constexpr uint32_t MAX_RAIN_PARTICLES = 65536;
    static constexpr uint32_t MAX_SNOW_PARTICLES = 32768;
    static constexpr uint32_t MAX_SPLASH_QUADS = 16384;
    static constexpr uint32_t LIGHTNING_SEGMENTS = 128;
};

} // namespace sphere::weather
