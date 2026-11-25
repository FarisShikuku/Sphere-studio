#pragma once

#include <vector>
#include "../../shared/math/MathTypes.h"

namespace procedural::props {

inline std::vector<Vec2> getTransformerCanProfile(float radius, float finDepth, int fins) {
    std::vector<Vec2> pts;
    int segments = fins * 4;
    pts.reserve(segments);
    float step = 6.2831853f / segments;
    for (int i = 0; i < segments; ++i) {
        float a = i * step;
        float r = radius + ((i % 4 == 1 || i % 4 == 2) ? finDepth : 0.0f);
        pts.push_back({std::cos(a) * r, std::sin(a) * r});
    }
    return pts;
}

} // namespace procedural::props
