#pragma once
#include <vulkan/vulkan.h>
#include "base/VulkanDevice.hpp"
namespace sphere::vk {
class Device {
public:
    Device();
    ~Device();
    VkDevice GetHandle() const { return m_LogicalDevice; }
    VkPhysicalDevice GetPhysical() const { return m_PhysicalDevice; }
    VkInstance GetInstance() const { return m_Instance; }
private:
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_LogicalDevice = VK_NULL_HANDLE;
};
}
