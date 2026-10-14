#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace sphere::vk {
class BindlessHeap {
public:
    static constexpr uint32_t MAX_BINDLESS_TEXTURES = 65536;
    static constexpr uint32_t MAX_BINDLESS_BUFFERS = 16384;

    VkDescriptorPool pool = VK_NULL_HANDLE;
    VkDescriptorSetLayout layout = VK_NULL_HANDLE;
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

    void Init(VkDevice device);
    uint32_t RegisterTexture(VkDevice device, VkImageView view, VkSampler sampler);
    void Destroy(VkDevice device);
};
}\n