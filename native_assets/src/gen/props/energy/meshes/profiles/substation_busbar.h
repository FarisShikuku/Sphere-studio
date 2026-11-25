#pragma once

#include <vector>
#include "../../shared/math/MathTypes.h"

namespace procedural::props {

struct SubstationBusbarSection {
    float outerRadius = 0.06f;
    float wallThickness = 0.008f;
    int segments = 12;
};

} // namespace procedural::props
