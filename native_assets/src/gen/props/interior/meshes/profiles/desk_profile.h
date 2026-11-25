#pragma once

#include <vector>
#include "../../../shared/math/MathTypes.h"

namespace procedural::props {

struct DesktopProfile {
    float desktopThickness = 0.035f;
    float edgeBevel = 0.005f;
    float legWidth = 0.05f;
    float drawerFrontThickness = 0.018f;
    float handleLength = 0.12f;
};

} // namespace procedural::props
