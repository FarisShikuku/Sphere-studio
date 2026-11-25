#pragma once

#include <vector>
#include "../../shared/math/MathTypes.h"

namespace procedural::props {

inline std::vector<Vec2> sampleTurbineTowerSection(float radius, int segments) {
    std::vector<Vec2> pts;
    pts.reserve(segments);
    float step = 6.2831853f / segments;
    for (int i = 0; i < segments; ++i) {
        float a = i * step;
        pts.push_back({std::cos(a) * radius, std::sin(a) * radius});
    }
    return pts;
}

} // namespace procedural::props
