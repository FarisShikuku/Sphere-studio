#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace sphere::base {
class VulkanSwapChain {
public:
    VkInstance instance = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    uint32_t imageCount = 0;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
    VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    VkExtent2D extent{1920, 1080};

    void InitSurface(void* windowHandle);
    void Create(uint32_t* width, uint32_t* height, bool vsync = true);
    VkResult AcquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t* imageIndex);
    VkResult QueuePresent(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE);
    void Cleanup();
};
}\n