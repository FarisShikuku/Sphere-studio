#include "VulkanTexture.hpp"

namespace sphere::base {
void VulkanTexture::Destroy() {
    if (view) {
        vkDestroyImageView(device, view, nullptr);
        view = VK_NULL_HANDLE;
    }
    if (image) {
        vkDestroyImage(device, image, nullptr);
        image = VK_NULL_HANDLE;
    }
    if (sampler) {
        vkDestroySampler(device, sampler, nullptr);
        sampler = VK_NULL_HANDLE;
    }
    if (deviceMemory) {
        vkFreeMemory(device, deviceMemory, nullptr);
        deviceMemory = VK_NULL_HANDLE;
    }
}
}\n