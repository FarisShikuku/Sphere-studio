#pragma once

#include <vector>
#include "../../../shared/math/MathTypes.h"

namespace procedural::props {

struct PalletBlockProfile {
    float blockWidth = 0.145f;
    float blockLength = 0.145f;
    float blockHeight = 0.078f;
    float topDeckThickness = 0.022f;
    float stringerBoardThickness = 0.022f;
    float bottomDeckThickness = 0.022f;
    float forkliftChamfer = 0.015f;
};

} // namespace procedural::props
