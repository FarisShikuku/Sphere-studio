#pragma once

#include <vector>
#include <cmath>
#include "../../../shared/math/MathTypes.h"

namespace procedural::props {

struct BusShelterCanopyProfile {
    float length = 3.6f;
    float depth = 1.6f;
    float canopyArcRise = 0.35f;
    float mullionSpacing = 1.2f;
    float glassThickness = 0.012f;
};

inline std::vector<Vec2> sampleCanopyArc(float depth, float rise, int steps) {
    std::vector<Vec2> pts;
    pts.reserve(steps + 1);
    for (int i = 0; i <= steps; ++i) {
        float t = static_cast<float>(i) / steps;
        float x = depth * t;
        float y = rise * std::sin(t * 3.1415926f);
        pts.push_back({x, y});
    }
    return pts;
}

} // namespace procedural::props
