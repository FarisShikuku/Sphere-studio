#pragma once
#include <vulkan/vulkan.h>
namespace sphere::base {
    // Vulkan 1.3 core dynamic function pointer loaders
    inline PFN_vkCmdBeginRenderingKHR vkCmdBeginRendering_fn = nullptr;
    inline PFN_vkCmdEndRenderingKHR vkCmdEndRendering_fn = nullptr;
}
