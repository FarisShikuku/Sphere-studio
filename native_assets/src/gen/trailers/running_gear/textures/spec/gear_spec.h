#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::trailers::gear {

constexpr uint32_t GEAR_TEX_WIDTH = 1024;
constexpr uint32_t GEAR_TEX_HEIGHT = 1024;
constexpr float GEAR_TIRE_RADIUS_M = 0.52f;
constexpr float GEAR_AXLE_SPACING_M = 1.31f;
constexpr float GEAR_SUSPENSION_TRAVEL_M = 0.18f;
constexpr VkFormat GEAR_ALBEDO_FORMAT = VK_FORMAT_BC7_SRGB_BLOCK;
constexpr VkFormat GEAR_NORMAL_FORMAT = VK_FORMAT_BC5_UNORM_BLOCK;
constexpr VkFormat GEAR_ORM_FORMAT = VK_FORMAT_BC7_UNORM_BLOCK;

} // namespace sphere::trailers::gear
