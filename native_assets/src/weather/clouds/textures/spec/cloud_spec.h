#pragma once
#include <cstdint>

namespace sphere::weather {

struct CloudSpec {
    static constexpr uint32_t SHAPE_NOISE_RES = 128;
    static constexpr uint32_t DETAIL_NOISE_RES = 32;
};

} // namespace sphere::weather
