#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::vehicles {
struct TractorTextureSpec {
    static constexpr uint32_t kWidth = 1024;
    static constexpr uint32_t kHeight = 1024;
    static constexpr float kFifthWheelDefaultOffsetM = 1.65f;
    static constexpr VkFormat kAlbedoFormat = VK_FORMAT_BC7_SRGB_BLOCK;
    static constexpr VkFormat kNormalFormat = VK_FORMAT_BC5_UNORM_BLOCK;
    static constexpr VkFormat kOrmFormat = VK_FORMAT_BC7_UNORM_BLOCK;
};
} // namespace sphere::vehicles
