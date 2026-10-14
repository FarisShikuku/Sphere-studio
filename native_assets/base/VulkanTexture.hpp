#pragma once
#include <vulkan/vulkan.h>

namespace sphere::base {
class VulkanTexture {
public:
    VkDevice device = VK_NULL_HANDLE;
    VkImage image = VK_NULL_HANDLE;
    VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t mipLevels = 1;
    uint32_t layerCount = 1;
    VkDescriptorImageInfo descriptor{};
    VkSampler sampler = VK_NULL_HANDLE;

    void Destroy();
};
}\n