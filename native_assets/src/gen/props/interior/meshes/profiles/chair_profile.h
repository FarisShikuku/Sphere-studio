#pragma once

#include <vector>
#include <cmath>
#include "../../../shared/math/MathTypes.h"

namespace procedural::props {

struct ChairErgonomicsProfile {
    float seatCushionThickness = 0.08f;
    float backrestLumbarCurve = 0.04f;
    float starBaseLegRadius = 0.32f;
    int starBaseArmCount = 5;
    float casterWheelRadius = 0.03f;
};

inline std::vector<Vec3> sampleChairStarBasePoints(float radius, int arms) {
    std::vector<Vec3> pts;
    pts.reserve(arms);
    for (int i = 0; i < arms; ++i) {
        float angle = (static_cast<float>(i) / arms) * 6.2831853f;
        pts.push_back({radius * std::cos(angle), 0.0f, radius * std::sin(angle)});
    }
    return pts;
}

} // namespace procedural::props
