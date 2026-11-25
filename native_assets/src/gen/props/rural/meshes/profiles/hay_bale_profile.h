#pragma once

#include <vector>
#include <cmath>
#include "../../../shared/math/MathTypes.h"

namespace procedural::props {

struct RoundBaleProfile {
    float radius = 0.75f;
    float length = 1.35f;
    float barrelSag = 0.05f; // Slight flattening on bottom contact
    int radialSegments = 24;
};

inline std::vector<Vec2> sampleRoundBaleContactContour(float r, float sag, int steps) {
    std::vector<Vec2> pts;
    pts.reserve(steps);
    for (int i = 0; i < steps; ++i) {
        float angle = (static_cast<float>(i) / steps) * 6.2831853f;
        float currentR = r;
        if (std::sin(angle) < -0.7f) {
            currentR -= sag * (-std::sin(angle) - 0.7f) / 0.3f;
        }
        pts.push_back({currentR * std::cos(angle), currentR * std::sin(angle)});
    }
    return pts;
}

} // namespace procedural::props
