#include "PuddleParams.h"

namespace sphere::weather {

float CalculatePuddleMask(float groundRoughness, float slope, float wetness) {
    if (slope > 0.15f) return 0.0f; // drains off steep slopes
    return (1.0f - groundRoughness) * wetness;
}

} // namespace sphere::weather
