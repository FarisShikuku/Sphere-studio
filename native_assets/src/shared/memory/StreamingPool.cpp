#include "StreamingPool.h"
#include "DeviceMemoryAllocator.h"

namespace sphere {

StreamingPool::~StreamingPool() {
    Shutdown();
}

void StreamingPool::Initialize(VkDevice device, VkPhysicalDevice physicalDevice, size_t poolSize) {
    m_Device = device;
    m_TotalSize = poolSize;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = poolSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateBuffer(device, &bufferInfo, nullptr, &m_StagingBuffer);

    VkMemoryPropertyFlags props = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    m_StagingMemory = DeviceMemoryAllocator::AllocateBufferMemory(device, physicalDevice, m_StagingBuffer, props);

    vkMapMemory(device, m_StagingMemory, 0, poolSize, 0, (void**)&m_MappedData);
}

void StreamingPool::Shutdown() {
    if (m_Device != VK_NULL_HANDLE) {
        if (m_MappedData) vkUnmapMemory(m_Device, m_StagingMemory);
        if (m_StagingBuffer != VK_NULL_HANDLE) vkDestroyBuffer(m_Device, m_StagingBuffer, nullptr);
        if (m_StagingMemory != VK_NULL_HANDLE) vkFreeMemory(m_Device, m_StagingMemory, nullptr);
        m_MappedData = nullptr;
        m_StagingBuffer = VK_NULL_HANDLE;
        m_StagingMemory = VK_NULL_HANDLE;
        m_Device = VK_NULL_HANDLE;
    }
}

void* StreamingPool::AllocateStaging(size_t size, VkBuffer& outBuffer, size_t& outOffset) {
    size_t aligned = (size + 63) & ~size_t(63);
    if (m_CurrentOffset + aligned > m_TotalSize) return nullptr;

    outBuffer = m_StagingBuffer;
    outOffset = m_CurrentOffset;
    void* ptr = m_MappedData + m_CurrentOffset;
    m_CurrentOffset += aligned;
    return ptr;
}

void StreamingPool::Reset() {
    m_CurrentOffset = 0;
}

} // namespace sphere
