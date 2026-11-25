#pragma once
#include <cstdint>

namespace vegetation {
struct GrassBladeProfile {
    float widthAtRoot{0.035f};
    float widthAtMid{0.022f};
    float widthAtTip{0.002f};
    float naturalCurvatureExp{2.2f};
    int lodSegments[4]{5, 4, 3, 2};
};
}
