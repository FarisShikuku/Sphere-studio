#pragma once

#include <vector>
#include "../../shared/math/MathTypes.h"

namespace procedural::props {

inline std::vector<Vec3> sampleStreetLightArmCurve(float reach, float rise, int steps) {
    std::vector<Vec3> pts;
    pts.reserve(steps + 1);
    for (int i = 0; i <= steps; ++i) {
        float t = static_cast<float>(i) / steps;
        float x = reach * t;
        float y = rise * std::sin(t * 1.5707963f);
        pts.push_back({x, y, 0.0f});
    }
    return pts;
}

} // namespace procedural::props
