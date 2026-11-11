#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::cargo::raw {

struct RawTextureSpec {
    uint32_t width = 1024;
    uint32_t height = 1024;
    VkFormat albedoFormat = VK_FORMAT_BC7_UNORM_SRGB_BLOCK;
    VkFormat normalFormat = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormat ormFormat = VK_FORMAT_BC7_UNORM_BLOCK;
};

struct RawDimensionsSpec {
    float lengthMeters = 2.40f;
    float diameterMeters = 0.45f;
    float barkThicknessMeters = 0.015f;
    float annualRingPitch = 0.006f;
};

} // namespace sphere::cargo::raw
