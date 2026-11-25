#pragma once

#include <vector>
#include <cmath>
#include "../../../shared/math/MathTypes.h"

namespace procedural::props {

struct MailboxCapProfile {
    float width = 0.55f;
    float depth = 0.50f;
    float archHeight = 0.18f;
    float dropChuteSlotWidth = 0.35f;
    float dropChuteSlotHeight = 0.08f;
};

inline std::vector<Vec2> sampleMailboxArchCap(float width, float archH, int steps) {
    std::vector<Vec2> pts;
    pts.reserve(steps + 1);
    float halfW = width * 0.5f;
    for (int i = 0; i <= steps; ++i) {
        float a = (static_cast<float>(i) / steps) * 3.1415926f;
        pts.push_back({-halfW * std::cos(a), archH * std::sin(a)});
    }
    return pts;
}

} // namespace procedural::props
