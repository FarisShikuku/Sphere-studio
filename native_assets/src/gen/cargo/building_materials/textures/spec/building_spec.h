#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::cargo::building {

constexpr uint32_t BUILDING_TEX_WIDTH = 1024;
constexpr uint32_t BUILDING_TEX_HEIGHT = 1024;
constexpr VkFormat BUILDING_ALBEDO_FORMAT = VK_FORMAT_BC7_SRGB_BLOCK;
constexpr VkFormat BUILDING_NORMAL_FORMAT = VK_FORMAT_BC5_UNORM_BLOCK;
constexpr VkFormat BUILDING_ORM_FORMAT = VK_FORMAT_BC7_UNORM_BLOCK;

} // namespace sphere::cargo::building
