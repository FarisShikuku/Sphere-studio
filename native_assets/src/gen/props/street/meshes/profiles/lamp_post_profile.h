#pragma once

#include <vector>
#include <cmath>
#include "../../../shared/math/MathTypes.h"

namespace procedural::props {

inline std::vector<Vec3> sampleLampPostArmArc(float reach, float archRise, int steps) {
    std::vector<Vec3> points;
    points.reserve(steps + 1);
    for (int i = 0; i <= steps; ++i) {
        float t = static_cast<float>(i) / steps;
        float x = reach * t;
        float y = archRise * std::sin(t * 1.5707963f);
        points.push_back({x, y, 0.0f});
    }
    return points;
}

struct LampPostPoleProfile {
    float baseRadius = 0.15f;
    float topRadius = 0.08f;
    float pedestalHeight = 0.45f;
    int segments = 16;
};

} // namespace procedural::props
