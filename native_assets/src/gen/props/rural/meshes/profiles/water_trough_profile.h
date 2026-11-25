#pragma once

#include <vector>
#include "../../../shared/math/MathTypes.h"

namespace procedural::props {

struct WaterTroughTubProfile {
    float length = 1.8f;
    float width = 0.65f;
    float height = 0.55f;
    float wallThickness = 0.04f;
    float rimBevel = 0.02f;
    float floatBallRadius = 0.07f;
};

} // namespace procedural::props
