#include "VulkanSwapChain.hpp"

namespace sphere::base {
void VulkanSwapChain::InitSurface(void* windowHandle) {
    // Surface init hook
}

void VulkanSwapChain::Create(uint32_t* width, uint32_t* height, bool vsync) {
    extent.width = *width;
    extent.height = *height;
}

VkResult VulkanSwapChain::AcquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t* imageIndex) {
    *imageIndex = 0;
    return VK_SUCCESS;
}

VkResult VulkanSwapChain::QueuePresent(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore) {
    return VK_SUCCESS;
}

void VulkanSwapChain::Cleanup() {
    for (auto& view : imageViews) {
        if (view) vkDestroyImageView(device, view, nullptr);
    }
    imageViews.clear();
    if (swapChain) {
        vkDestroySwapchainKHR(device, swapChain, nullptr);
        swapChain = VK_NULL_HANDLE;
    }
}
}\n