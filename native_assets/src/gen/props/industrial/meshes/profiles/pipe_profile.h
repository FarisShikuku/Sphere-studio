#pragma once

#include <vector>
#include <cmath>
#include "../../../shared/math/MathTypes.h"

namespace procedural::props {

struct PipeFlangeProfile {
    float flangeRadius = 0.22f;
    float flangeThickness = 0.035f;
    int boltCount = 8;
    float boltRadius = 0.012f;
    float boltCircleRadius = 0.185f;
};

inline std::vector<Vec3> samplePipeElbowCurve(float bendRadius, int steps) {
    std::vector<Vec3> pts;
    pts.reserve(steps + 1);
    for (int i = 0; i <= steps; ++i) {
        float angle = (static_cast<float>(i) / steps) * 1.5707963f; // 90 degrees
        pts.push_back({
            bendRadius * (1.0f - std::cos(angle)),
            bendRadius * std::sin(angle),
            0.0f
        });
    }
    return pts;
}

} // namespace procedural::props
