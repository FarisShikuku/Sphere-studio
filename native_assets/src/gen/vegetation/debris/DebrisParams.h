#pragma once
// ============================================================================
// DebrisParams.h - Forest Floor Organic Litter Types in C++17
// ============================================================================

#include <cstdint>

namespace vegetation {

enum class DebrisType : uint32_t {
    FALLEN_TWIG = 0,
    PINE_CONE = 1,
    DEAD_LEAF = 2,
    DECAYING_BARK = 3,
    FOREST_PEBBLE = 4
};

struct DebrisScatterConfig {
    DebrisType type{DebrisType::FALLEN_TWIG};
    float densityPerSqMeter{3.5f};
    float minScale{0.08f};
    float maxScale{0.32f};
    float humidityWetness{0.4f};
};

} // namespace vegetation
