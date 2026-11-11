#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::cargo::perishable {

struct PerishableTextureSpec {
    uint32_t width = 1024;
    uint32_t height = 1024;
    VkFormat albedoFormat = VK_FORMAT_BC7_UNORM_SRGB_BLOCK;
    VkFormat normalFormat = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormat ormFormat = VK_FORMAT_BC7_UNORM_BLOCK;
};

struct PerishableDimensionsSpec {
    float lengthMeters = 0.50f;
    float widthMeters = 0.35f;
    float heightMeters = 0.25f;
    float produceRadius = 0.042f;
    float iceFlakeSize = 0.008f;
};

} // namespace sphere::cargo::perishable
