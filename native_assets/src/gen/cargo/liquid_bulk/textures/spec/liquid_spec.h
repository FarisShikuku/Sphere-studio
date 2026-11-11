#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::cargo::liquid {

struct LiquidTextureSpec {
    uint32_t width = 1024;
    uint32_t height = 1024;
    VkFormat albedoFormat = VK_FORMAT_BC7_SRGB_BLOCK;
    VkFormat normalFormat = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormat ormFormat = VK_FORMAT_BC7_UNORM_BLOCK;
};

struct LiquidDimensionsSpec {
    float diameterMeters = 0.58f;
    float heightMeters = 0.88f;
    float nominalCapacityLiters = 200.0f;
    float ribHeight = 0.016f;
    float chimeThickness = 0.012f;
};

} // namespace sphere::cargo::liquid
