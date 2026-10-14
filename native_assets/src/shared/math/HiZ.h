#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>
#include <vector>
#include "scene/Transform.h"
#include "scene/AABB.h"

namespace sphere {

struct HizBuffer {
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t mipLevels = 0;
    std::vector<std::vector<float>> mips;
};

class HiZ {
public:
    // Vulkan GPU barrier transitions for Hi-Z mip chain downsampling
    static void BuildMipChain(VkCommandBuffer cmd, VkImage depthImage, uint32_t width, uint32_t height, uint32_t mipLevels);

    // CPU-side reference min-reduction pyramid generator
    static void BuildCpuHiZ(const float* rawDepth, uint32_t width, uint32_t height, HizBuffer& outHiz);

    // Basic depth occlusion check (reverse-Z: occluded if boxDepth < minDepth; standard: boxDepth > minDepth)
    static bool TestOcclusion(float minDepth, float boxDepth);

    // Full Screen-Space Bounding Box projection and Hi-Z buffer sample
    static bool TestOcclusionAABB(const AABB& aabb, const Mat4& viewProjection, const HizBuffer& hiz);

    // Projects 8 world-space AABB corners into screen UV [0, 1] and calculates nearest NDC depth
    static bool ProjectAABB(const AABB& aabb, const Mat4& viewProjection, float& minU, float& minV, float& maxU, float& maxV, float& nearestDepth);
};

} // namespace sphere
