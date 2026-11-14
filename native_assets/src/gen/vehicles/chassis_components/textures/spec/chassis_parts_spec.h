#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::vehicles::chassis {
struct ChassisPartsSpec {
    static constexpr uint32_t kWidth = 512;
    static constexpr uint32_t kHeight = 512;
    static constexpr VkFormat kAlbedoFormat = VK_FORMAT_BC7_SRGB_BLOCK;
    static constexpr VkFormat kNormalFormat = VK_FORMAT_BC5_UNORM_BLOCK;
    static constexpr VkFormat kOrmFormat = VK_FORMAT_BC7_UNORM_BLOCK;
};
} // namespace sphere::vehicles::chassis
