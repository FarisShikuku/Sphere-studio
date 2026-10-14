#include "BindlessHeap.h"

namespace sphere::vk {
void BindlessHeap::Init(VkDevice device) {}
uint32_t BindlessHeap::RegisterTexture(VkDevice device, VkImageView view, VkSampler sampler) { return 0; }
void BindlessHeap::Destroy(VkDevice device) {
    if (layout) vkDestroyDescriptorSetLayout(device, layout, nullptr);
    if (pool) vkDestroyDescriptorPool(device, pool, nullptr);
}
}\n