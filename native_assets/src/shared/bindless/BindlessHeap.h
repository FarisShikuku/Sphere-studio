#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>
#include <mutex>

namespace sphere {

class BindlessHeap {
public:
    static BindlessHeap& Get();

    void Initialize(VkDevice device, uint32_t maxTextures = 4096);
    void Shutdown();

    uint32_t RegisterTexture(VkImageView imageView, VkSampler sampler);
    void UnregisterTexture(uint32_t index);

    VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }
    VkDescriptorSetLayout GetLayout() const { return m_Layout; }

private:
    BindlessHeap() = default;
    ~BindlessHeap() = default;

    VkDevice m_Device = VK_NULL_HANDLE;
    VkDescriptorPool m_Pool = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_Layout = VK_NULL_HANDLE;
    VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;

    std::vector<uint32_t> m_FreeSlots;
    uint32_t m_NextSlot = 1; // 0 reserved for fallback white texture
    uint32_t m_MaxTextures = 4096;
    std::mutex m_Mutex;
};

} // namespace sphere
