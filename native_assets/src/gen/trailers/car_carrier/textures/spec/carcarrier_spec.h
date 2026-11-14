#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::trailers::car_carrier {
constexpr uint32_t CARCARRIER_TEX_WIDTH = 1024;
constexpr uint32_t CARCARRIER_TEX_HEIGHT = 1024;
constexpr VkFormat CARCARRIER_ALBEDO_FORMAT = VK_FORMAT_BC7_SRGB_BLOCK;
constexpr VkFormat CARCARRIER_NORMAL_FORMAT = VK_FORMAT_BC5_UNORM_BLOCK;
constexpr VkFormat CARCARRIER_ORM_FORMAT = VK_FORMAT_BC7_UNORM_BLOCK;
constexpr uint32_t CARCARRIER_MAX_VEHICLES = 8u;
constexpr float CARCARRIER_RAMP_SLOPE_DEG = 14.5f;
}
