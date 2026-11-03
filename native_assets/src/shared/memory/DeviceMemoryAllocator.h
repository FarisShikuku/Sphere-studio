#pragma once
#include <vulkan/vulkan.h>
#include "VulkanTools.hpp"

namespace sphere {

class DeviceMemoryAllocator {
public:
    static VkDeviceMemory AllocateBufferMemory(VkDevice device, VkPhysicalDevice physicalDevice, VkBuffer buffer, VkMemoryPropertyFlags props) {
        VkMemoryRequirements memReqs;
        vkGetBufferMemoryRequirements(device, buffer, &memReqs);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReqs.size;
        allocInfo.memoryTypeIndex = base::tools::FindMemoryType(physicalDevice, memReqs.memoryTypeBits, props);

        VkDeviceMemory mem = VK_NULL_HANDLE;
        vkAllocateMemory(device, &allocInfo, nullptr, &mem);
        if (mem != VK_NULL_HANDLE) {
            vkBindBufferMemory(device, buffer, mem, 0);
        }
        return mem;
    }
};

} // namespace sphere
