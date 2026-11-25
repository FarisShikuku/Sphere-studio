#pragma once
#include <cstdint>

namespace vegetation {
struct BushTextureSpec {
    uint32_t atlasResolution{2048};
    uint32_t leafVariantCount{8};
    float subsurfaceScatteringFactor{0.35f};
    float roughnessMean{0.65f};
};
}
