#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::cargo::bagged {

struct BagTextureSpec {
    uint32_t width = 1024;
    uint32_t height = 1024;
    VkFormat albedoFormat = VK_FORMAT_BC7_SRGB_BLOCK;
    VkFormat normalFormat = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormat ormFormat = VK_FORMAT_BC7_UNORM_BLOCK;
    uint32_t mipLevels = 11;
};

struct BagDimensionsSpec {
    float lengthMeters = 0.82f;
    float widthMeters = 0.48f;
    float thicknessMeters = 0.22f;
    float nominalMassKg = 50.0f;
    float seamInset = 0.035f;
    float stitchPitch = 0.012f;
};

} // namespace sphere::cargo::bagged
