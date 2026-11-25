#pragma once

#include <vector>
#include <cmath>
#include "../../shared/math/MathTypes.h"

namespace procedural::props {

// NACA 4-digit symmetric/cambered airfoil generator
inline std::vector<Vec2> sampleNacaAirfoil(float chord, float thicknessRatio, int sampleCount) {
    std::vector<Vec2> pts;
    pts.reserve(sampleCount * 2);
    for (int i = 0; i <= sampleCount; ++i) {
        float x = (static_cast<float>(i) / sampleCount) * chord;
        float xOverC = x / chord;
        float yt = 5.0f * thicknessRatio * chord * (
            0.2969f * std::sqrt(xOverC) -
            0.1260f * xOverC -
            0.3516f * std::pow(xOverC, 2.0f) +
            0.2843f * std::pow(xOverC, 3.0f) -
            0.1015f * std::pow(xOverC, 4.0f)
        );
        pts.push_back({x, yt});
    }
    return pts;
}

} // namespace procedural::props
