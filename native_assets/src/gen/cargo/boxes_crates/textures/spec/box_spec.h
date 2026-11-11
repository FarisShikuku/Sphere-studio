#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::cargo::boxes {

struct BoxTextureSpec {
    uint32_t width = 1024;
    uint32_t height = 1024;
    VkFormat albedoFormat = VK_FORMAT_BC7_SRGB_BLOCK;
    VkFormat normalFormat = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormat ormFormat = VK_FORMAT_BC7_UNORM_BLOCK;
};

struct BoxDimensionsSpec {
    float lengthMeters = 0.60f;
    float widthMeters = 0.40f;
    float heightMeters = 0.35f;
    float wallThickness = 0.005f; // Cardboard or slat thickness
    float cornerRadius = 0.008f;
};

} // namespace sphere::cargo::boxes
