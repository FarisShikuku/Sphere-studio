#pragma once
#include <cstdint>

namespace vegetation {
struct RockTextureSpec {
    uint32_t albedoMapRes{2048};
    uint32_t normalMapRes{2048};
    uint32_t roughnessMapRes{2048};
    float lichenScale{0.12f};
    float mossNormalOcclusion{0.8f};
};
}
