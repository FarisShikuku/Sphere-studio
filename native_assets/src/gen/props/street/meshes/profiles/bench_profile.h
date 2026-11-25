#pragma once

#include <vector>
#include "../../../shared/math/MathTypes.h"

namespace procedural::props {

struct BenchSlatProfile {
    float slatWidth = 0.08f;
    float slatThickness = 0.025f;
    float slatSpacing = 0.015f;
    int slatCount = 5;
    float seatHeight = 0.45f;
    float backrestAngleDeg = 12.0f;
};

inline std::vector<Vec2> getBenchLegContour() {
    return {
        {0.0f, 0.0f},
        {0.05f, 0.45f},
        {0.45f, 0.45f},
        {0.50f, 0.0f},
        {0.44f, 0.0f},
        {0.40f, 0.40f},
        {0.10f, 0.40f},
        {0.06f, 0.0f}
    };
}

} // namespace procedural::props
