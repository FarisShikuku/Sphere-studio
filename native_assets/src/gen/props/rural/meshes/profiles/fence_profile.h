#pragma once

#include <vector>
#include "../../../shared/math/MathTypes.h"

namespace procedural::props {

struct PicketProfile {
    float width = 0.075f;
    float thickness = 0.02f;
    float spearPointHeight = 0.06f;
};

inline std::vector<Vec2> getPicketSpearContour(float w, float h, float pointH) {
    float halfW = w * 0.5f;
    return {
        {-halfW, 0.0f},
        {-halfW, h - pointH},
        {0.0f, h},
        {halfW, h - pointH},
        {halfW, 0.0f}
    };
}

} // namespace procedural::props
