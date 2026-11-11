#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::cargo::bottles {

struct BottleTextureSpec {
    uint32_t width = 1024;
    uint32_t height = 1024;
    VkFormat albedoFormat = VK_FORMAT_BC7_SRGB_BLOCK;
    VkFormat normalFormat = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormat ormFormat = VK_FORMAT_BC7_UNORM_BLOCK;
};

struct BottleDimensionsSpec {
    float diameterMeters = 0.062f;
    float heightMeters = 0.228f;
    float glassWallThickness = 0.003f;
    float moldSeamWidth = 0.0008f;
};

} // namespace sphere::cargo::bottles
