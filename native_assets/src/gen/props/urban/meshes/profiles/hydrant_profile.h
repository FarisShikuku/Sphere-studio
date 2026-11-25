#pragma once

#include <vector>
#include <cmath>
#include "../../../shared/math/MathTypes.h"

namespace procedural::props {

struct HydrantBonnetProfile {
    float flangeRadius = 0.16f;
    float domeRadius = 0.13f;
    float domeHeight = 0.10f;
    float nutRadius = 0.035f;
    float nutHeight = 0.04f;
};

inline std::vector<Vec2> sampleHydrantBonnetDome(float r, float h, int steps) {
    std::vector<Vec2> pts;
    pts.reserve(steps + 1);
    for (int i = 0; i <= steps; ++i) {
        float angle = (static_cast<float>(i) / steps) * 1.5707963f; // 0 to 90 deg
        pts.push_back({r * std::cos(angle), h * std::sin(angle)});
    }
    return pts;
}

} // namespace procedural::props
