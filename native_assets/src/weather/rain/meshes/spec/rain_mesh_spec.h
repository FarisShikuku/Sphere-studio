#pragma once
#include <cstdint>

namespace sphere::weather {

struct RainMeshSpec {
    static constexpr uint32_t VERTICES_PER_STREAK = 4;
    static constexpr uint32_t INDICES_PER_STREAK = 6;
};

} // namespace sphere::weather
