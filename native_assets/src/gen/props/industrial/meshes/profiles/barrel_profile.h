#pragma once

#include <vector>
#include <cmath>
#include "../../../shared/math/MathTypes.h"

namespace procedural::props {

struct BarrelContourProfile {
    float topRimRadius = 0.285f;
    float bellyRadius = 0.30f;
    float bottomRimRadius = 0.285f;
    float chimeHeight = 0.02f;
    float rollingHoopOffset1 = 0.30f;
    float rollingHoopOffset2 = 0.60f;
};

inline std::vector<Vec2> sampleBarrelRibProfile(float height, float minR, float maxR, int steps) {
    std::vector<Vec2> pts;
    pts.reserve(steps + 1);
    for (int i = 0; i <= steps; ++i) {
        float t = static_cast<float>(i) / steps;
        float y = height * t;
        // parabolic belly bulge
        float normalizedY = (t - 0.5f) * 2.0f; // -1 to 1
        float r = maxR - (maxR - minR) * (normalizedY * normalizedY);
        pts.push_back({r, y});
    }
    return pts;
}

} // namespace procedural::props
