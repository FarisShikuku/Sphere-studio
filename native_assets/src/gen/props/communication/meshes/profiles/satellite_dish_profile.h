#pragma once

#include <vector>
#include <cmath>
#include "../../shared/math/MathTypes.h"

namespace procedural::props {

inline std::vector<Vec2> sampleParabolicCurve(float radius, float focalLength, int samples) {
    std::vector<Vec2> pts;
    pts.reserve(samples + 1);
    for (int i = 0; i <= samples; ++i) {
        float r = (static_cast<float>(i) / samples) * radius;
        float z = (r * r) / (4.0f * focalLength);
        pts.push_back({r, z});
    }
    return pts;
}

} // namespace procedural::props
