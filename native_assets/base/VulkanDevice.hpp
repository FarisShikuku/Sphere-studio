#pragma once
#include <vulkan/vulkan.h>
#include "VulkanBuffer.hpp"

namespace sphere::base {
class VulkanDevice {
public:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties properties{};
    VkPhysicalDeviceFeatures features{};
    VkPhysicalDeviceMemoryProperties memoryProperties{};
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue computeQueue = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;

    explicit VulkanDevice(VkPhysicalDevice pDevice);
    ~VulkanDevice();

    VkResult CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, VulkanBuffer* buffer, void* data = nullptr);
    VkCommandBuffer CreateCommandBuffer(VkCommandBufferLevel level, bool begin);
    void FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, bool free = true);
};
}\n