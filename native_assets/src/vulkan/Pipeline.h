#pragma once
#include <vulkan/vulkan.h>

namespace sphere::vk {
class Pipeline {
public:
    VkPipeline handle = VK_NULL_HANDLE;
    VkPipelineLayout layout = VK_NULL_HANDLE;
    VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    void Destroy(VkDevice device) {
        if (handle) { vkDestroyPipeline(device, handle, nullptr); handle = VK_NULL_HANDLE; }
        if (layout) { vkDestroyPipelineLayout(device, layout, nullptr); layout = VK_NULL_HANDLE; }
    }
};
}\n