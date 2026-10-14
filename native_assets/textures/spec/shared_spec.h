#pragma once
#include <vulkan/vulkan.h>

namespace sphere::textures {
struct TextureFormatSpec {
    VkFormat albedoFormat = VK_FORMAT_BC7_UNORM_BLOCK;
    VkFormat normalFormat = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormat ormFormat = VK_FORMAT_BC7_UNORM_BLOCK;
    bool generateMips = true;
};
}\n