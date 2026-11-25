#pragma once
// ============================================================================
// WindParams.h - Atmospheric Flow & Gust Parameters in C++17
// ============================================================================

#include <cstdint>

namespace vegetation {

struct WindSynthesisParams {
    float baseSpeedMps{4.5f};          // Meters per second
    float windDirectionRad{0.785398f}; // Direction angle (45 degrees)
    float gustFrequencyHz{0.22f};      // Wave oscillation period
    float gustAmplitude{2.8f};         // Gust surge
    float turbulenceScale{0.15f};      // Spatial frequency
    float plantStiffnessDamping{0.82f};
};

} // namespace vegetation
