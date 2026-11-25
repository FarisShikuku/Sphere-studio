#pragma once
#include <cstdint>

namespace vegetation {
struct WindTextureSpec {
    uint32_t vectorFieldTextureRes{256}; // 3D volume RGBA16F
    float spatialScaleMeters{64.0f};
    float temporalPeriodSec{10.0f};
};
}
