#pragma once

#include <vector>
#include "../../../shared/math/MathTypes.h"

namespace procedural::props {

struct TrafficLightHousingProfile {
    float signalHeadWidth = 0.35f;
    float signalHeadHeight = 1.05f;
    float signalHeadDepth = 0.28f;
    float visorLength = 0.25f;
    float lensRadius = 0.10f;
    int sectionCount = 3; // Red, Yellow, Green
};

} // namespace procedural::props
