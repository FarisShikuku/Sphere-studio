#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::trailers::chassis {
constexpr uint32_t CHASSIS_TEX_WIDTH = 1024;
constexpr uint32_t CHASSIS_TEX_HEIGHT = 1024;
constexpr VkFormat CHASSIS_ALBEDO_FORMAT = VK_FORMAT_BC7_SRGB_BLOCK;
constexpr VkFormat CHASSIS_NORMAL_FORMAT = VK_FORMAT_BC5_UNORM_BLOCK;
constexpr VkFormat CHASSIS_ORM_FORMAT = VK_FORMAT_BC7_UNORM_BLOCK;
constexpr float CHASSIS_TWISTLOCK_SPACING_M = 11.985f;
constexpr float CHASSIS_KINGPIN_HEIGHT_M = 1.18f;
}
