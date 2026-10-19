#pragma once
#include <algorithm>

namespace sphere::weather {

inline float EvaluateFlashCurve(float normalizedTime) {
    if (normalizedTime < 0.1f) return normalizedTime / 0.1f;
    return std::max(0.0f, 1.0f - (normalizedTime - 0.1f) / 0.9f);
}

} // namespace sphere::weather
