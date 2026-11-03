#include "VulkanBuffer.hpp"
#include <cstring>
namespace sphere::base {
VkResult VulkanBuffer::Map(VkDeviceSize mapSize, VkDeviceSize offset) {
    return vkMapMemory(device, memory, offset, mapSize, 0, &mapped);
}
void VulkanBuffer::Unmap() {
    if (mapped) {
        vkUnmapMemory(device, memory);
        mapped = nullptr;
    }
}
VkResult VulkanBuffer::Bind(VkDeviceSize offset) {
    return vkBindBufferMemory(device, buffer, memory, offset);
}
void VulkanBuffer::SetupDescriptor(VkDeviceSize descSize, VkDeviceSize offset) {
    descriptor.offset = offset;
    descriptor.buffer = buffer;
    descriptor.range = descSize;
}
void VulkanBuffer::CopyTo(void* data, VkDeviceSize copySize) {
    if (mapped && data) {
        memcpy(mapped, data, copySize);
    }
}
VkResult VulkanBuffer::Flush(VkDeviceSize flushSize, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange{};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory;
    mappedRange.offset = offset;
    mappedRange.size = flushSize;
    return vkFlushMappedMemoryRanges(device, 1, &mappedRange);
}
void VulkanBuffer::Destroy() {
    if (buffer) {
        vkDestroyBuffer(device, buffer, nullptr);
        buffer = VK_NULL_HANDLE;
    }
    if (memory) {
        vkFreeMemory(device, memory, nullptr);
        memory = VK_NULL_HANDLE;
    }
}
}
