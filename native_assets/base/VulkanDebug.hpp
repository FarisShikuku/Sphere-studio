#pragma once
#include <vulkan/vulkan.h>

namespace sphere::base::debug {
    void SetupDebugMessenger(VkInstance instance);
    void DestroyDebugMessenger(VkInstance instance);
}\n