#include "HiZ.h"
#include <cmath>
#include <algorithm>

namespace sphere {

bool HiZ::TestOcclusion(float minDepth, float boxDepth) {
    // In standard Vulkan depth [0 = near, 1 = far]:
    // If the closest point of the bounding box is farther than the maximum depth observed in that tile, it is occluded.
    return boxDepth > minDepth;
}

void HiZ::BuildCpuHiZ(const float* rawDepth, uint32_t width, uint32_t height, HizBuffer& outHiz) {
    if (!rawDepth || width == 0 || height == 0) return;

    outHiz.width = width;
    outHiz.height = height;
    outHiz.mips.clear();

    // Calculate mip levels
    uint32_t maxDim = std::max(width, height);
    uint32_t levels = 1;
    while ((1u << levels) <= maxDim) {
        levels++;
    }
    outHiz.mipLevels = levels;
    outHiz.mips.resize(levels);

    // Mip 0 copy
    outHiz.mips[0].assign(rawDepth, rawDepth + (width * height));

    uint32_t prevW = width;
    uint32_t prevH = height;

    for (uint32_t lvl = 1; lvl < levels; ++lvl) {
        uint32_t curW = std::max(1u, prevW / 2);
        uint32_t curH = std::max(1u, prevH / 2);
        outHiz.mips[lvl].resize(curW * curH);

        const auto& prevMip = outHiz.mips[lvl - 1];

        for (uint32_t y = 0; y < curH; ++y) {
            for (uint32_t x = 0; x < curW; ++x) {
                uint32_t x0 = x * 2;
                uint32_t x1 = std::min(x0 + 1, prevW - 1);
                uint32_t y0 = y * 2;
                uint32_t y1 = std::min(y0 + 1, prevH - 1);

                float d00 = prevMip[y0 * prevW + x0];
                float d10 = prevMip[y0 * prevW + x1];
                float d01 = prevMip[y1 * prevW + x0];
                float d11 = prevMip[y1 * prevW + x1];

                // Conservative occlusion culling: min reduction in [0..1]
                float minVal = std::min(std::min(d00, d10), std::min(d01, d11));
                outHiz.mips[lvl][y * curW + x] = minVal;
            }
        }
        prevW = curW;
        prevH = curH;
    }
}

} // namespace sphere
