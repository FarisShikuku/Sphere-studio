#pragma once
#include <cstdint>

namespace sphere {

enum class GrassSurfaceVariant : uint32_t {
    MONDO_LAWN_SURFACE = 0, // Flat surface texture (low poly)
    WILD_MEADOW_SURFACE,
    DRY_SAVANNA_SURFACE,
    WET_GOLF_TURF_SURFACE
};

} // namespace sphere
