#pragma once
#include <cstdint>

namespace vegetation {
struct CropRowProfile {
    float furrowDepth{0.14f};
    float ridgeWidth{0.45f};
    float valleyWidth{0.45f};
    float soilRoughness{0.05f};
};
}
