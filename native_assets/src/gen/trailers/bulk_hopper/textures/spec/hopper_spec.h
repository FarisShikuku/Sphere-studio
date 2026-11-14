#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::trailers::bulk_hopper {
constexpr uint32_t HOPPER_TEX_WIDTH = 1024;
constexpr uint32_t HOPPER_TEX_HEIGHT = 1024;
constexpr VkFormat HOPPER_ALBEDO_FORMAT = VK_FORMAT_BC7_SRGB_BLOCK;
constexpr VkFormat HOPPER_NORMAL_FORMAT = VK_FORMAT_BC5_UNORM_BLOCK;
constexpr VkFormat HOPPER_ORM_FORMAT = VK_FORMAT_BC7_UNORM_BLOCK;
constexpr float HOPPER_VOLUME_M3 = 48.0f;
constexpr float HOPPER_DISCHARGE_RATE_TPH = 65.0f;
constexpr float HOPPER_PRESSURE_BAR = 2.0f;
}
