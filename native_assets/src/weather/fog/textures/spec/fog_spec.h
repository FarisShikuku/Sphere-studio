#pragma once
#include <cstdint>

namespace sphere::weather {

struct FogSpec {
    static constexpr uint32_t FROXEL_GRID_X = 160;
    static constexpr uint32_t FROXEL_GRID_Y = 90;
    static constexpr uint32_t FROXEL_GRID_Z = 64;
};

} // namespace sphere::weather
