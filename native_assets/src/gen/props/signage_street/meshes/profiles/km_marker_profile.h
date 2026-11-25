#pragma once

#include <vector>
#include <cmath>
#include "../../shared/math/MathTypes.h"

namespace procedural::props {

inline std::vector<Vec2> sampleMilestoneArch(float width, int steps) {
    std::vector<Vec2> pts;
    pts.reserve(steps + 1);
    float radius = width * 0.5f;
    for (int i = 0; i <= steps; ++i) {
        float a = (static_cast<float>(i) / steps) * 3.1415926f;
        pts.push_back({std::cos(a) * radius, std::sin(a) * radius});
    }
    return pts;
}

} // namespace procedural::props
