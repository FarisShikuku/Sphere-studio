#include "VulkanDebug.hpp"
#include <iostream>

namespace sphere::base::debug {
static VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    std::cerr << "[Vulkan Validation Layer]: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

void SetupDebugMessenger(VkInstance instance) {
    // Setup debug utils callback hook
}

void DestroyDebugMessenger(VkInstance instance) {
    // Cleanup debug messenger hook
}
}\n