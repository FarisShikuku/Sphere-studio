#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::cargo::specialty {

struct SpecialtyTextureSpec {
    uint32_t width = 1024;
    uint32_t height = 1024;
    VkFormat albedoFormat = VK_FORMAT_BC7_SRGB_BLOCK;
    VkFormat normalFormat = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormat ormFormat = VK_FORMAT_BC7_UNORM_BLOCK;
};

struct SpecialtyDimensionsSpec {
    float lengthMeters = 6.00f;
    float diameterMeters = 0.22f;
    float wallThicknessMeters = 0.008f;
    float palletShrinkWrapThickness = 0.001f;
};

} // namespace sphere::cargo::specialty
