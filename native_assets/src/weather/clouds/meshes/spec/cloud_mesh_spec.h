#pragma once
#include <cstdint>

namespace sphere::weather {

struct CloudMeshSpec {
    static constexpr uint32_t DOME_SLICES = 32;
    static constexpr uint32_t DOME_STACKS = 16;
};

} // namespace sphere::weather
