#pragma once
#include <vulkan/vulkan.h>
#include "scene/WorldState.h"

namespace sphere {
struct GeneratorContext {
    VkDevice device = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    uint32_t seed = 1337;
    float lodLevel = 0.0f;
    const WorldState* worldState = nullptr;
};
}\n