#include "VulkanDevice.hpp"
#include "VulkanTools.hpp"
#include "VulkanInitializers.hpp"

namespace sphere::base {
VulkanDevice::VulkanDevice(VkPhysicalDevice pDevice) : physicalDevice(pDevice) {
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
}

VulkanDevice::~VulkanDevice() {
    if (commandPool) {
        vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
    }
    if (logicalDevice) {
        vkDestroyDevice(logicalDevice, nullptr);
    }
}

VkResult VulkanDevice::CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, VulkanBuffer* buffer, void* data) {
    buffer->device = logicalDevice;
    buffer->size = size;
    buffer->usageFlags = usageFlags;
    buffer->memoryPropertyFlags = memoryPropertyFlags;

    VkBufferCreateInfo bufCreateInfo{};
    bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufCreateInfo.size = size;
    bufCreateInfo.usage = usageFlags;
    bufCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VK_CHECK_RESULT(vkCreateBuffer(logicalDevice, &bufCreateInfo, nullptr, &buffer->buffer));

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(logicalDevice, buffer->buffer, &memReqs);

    VkMemoryAllocateInfo memAlloc = initializers::MemoryAllocateInfo();
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = tools::FindMemoryType(physicalDevice, memReqs.memoryTypeBits, memoryPropertyFlags);
    VK_CHECK_RESULT(vkAllocateMemory(logicalDevice, &memAlloc, nullptr, &buffer->memory));

    if (data) {
        VK_CHECK_RESULT(buffer->Map());
        buffer->CopyTo(data, size);
        if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) {
            buffer->Flush();
        }
        buffer->Unmap();
    }

    VK_CHECK_RESULT(buffer->Bind());
    buffer->SetupDescriptor();
    return VK_SUCCESS;
}

VkCommandBuffer VulkanDevice::CreateCommandBuffer(VkCommandBufferLevel level, bool begin) {
    VkCommandBufferAllocateInfo cmdBufAllocInfo = initializers::CommandBufferAllocateInfo(commandPool, level, 1);
    VkCommandBuffer cmdBuffer;
    VK_CHECK_RESULT(vkAllocateCommandBuffers(logicalDevice, &cmdBufAllocInfo, &cmdBuffer));
    if (begin) {
        VkCommandBufferBeginInfo cmdBufInfo = initializers::CommandBufferBeginInfo();
        VK_CHECK_RESULT(vkBeginCommandBuffer(cmdBuffer, &cmdBufInfo));
    }
    return cmdBuffer;
}

void VulkanDevice::FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, bool free) {
    if (commandBuffer == VK_NULL_HANDLE) return;
    VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

    VkSubmitInfo submitInfo = initializers::SubmitInfo();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence fence;
    VK_CHECK_RESULT(vkCreateFence(logicalDevice, &fenceInfo, nullptr, &fence));
    VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, fence));
    VK_CHECK_RESULT(vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, 100000000000));
    vkDestroyFence(logicalDevice, fence, nullptr);

    if (free) {
        vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
    }
}
}\n