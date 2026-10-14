#pragma once
#include <vulkan/vulkan.h>

namespace sphere::meshes {
struct VertexLayoutSpec {
    uint32_t stride = 48; // Position(12) + Normal(12) + Tangent(16) + UV0(8)
    VkFormat positionFormat = VK_FORMAT_R32G32B32_SFLOAT;
    VkFormat normalFormat = VK_FORMAT_R32G32B32_SFLOAT;
    VkFormat tangentFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
    VkFormat uvFormat = VK_FORMAT_R32G32_SFLOAT;
};
}\n