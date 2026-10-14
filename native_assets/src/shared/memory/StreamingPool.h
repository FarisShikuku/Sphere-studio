#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>
#include <vector>

namespace sphere {

class StreamingPool {
public:
    StreamingPool() = default;
    ~StreamingPool();

    void Initialize(VkDevice device, VkPhysicalDevice physicalDevice, size_t poolSize = 64 * 1024 * 1024);
    void Shutdown();

    void* AllocateStaging(size_t size, VkBuffer& outBuffer, size_t& outOffset);
    void Reset();

private:
    VkDevice m_Device = VK_NULL_HANDLE;
    VkBuffer m_StagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_StagingMemory = VK_NULL_HANDLE;
    uint8_t* m_MappedData = nullptr;
    size_t m_TotalSize = 0;
    size_t m_CurrentOffset = 0;
};

} // namespace sphere
