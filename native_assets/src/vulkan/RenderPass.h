#pragma once
#include <vulkan/vulkan.h>

namespace sphere::vk {
class RenderPass {
public:
    VkRenderPass handle = VK_NULL_HANDLE;
    void Destroy(VkDevice device) {
        if (handle) {
            vkDestroyRenderPass(device, handle, nullptr);
            handle = VK_NULL_HANDLE;
        }
    }
};
}\n