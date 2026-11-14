#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::trailers::box_van {
constexpr uint32_t BOXVAN_TEX_WIDTH = 1024;
constexpr uint32_t BOXVAN_TEX_HEIGHT = 1024;
constexpr VkFormat BOXVAN_ALBEDO_FORMAT = VK_FORMAT_BC7_SRGB_BLOCK;
constexpr VkFormat BOXVAN_NORMAL_FORMAT = VK_FORMAT_BC5_UNORM_BLOCK;
constexpr VkFormat BOXVAN_ORM_FORMAT = VK_FORMAT_BC7_UNORM_BLOCK;
constexpr float BOXVAN_INTERIOR_WIDTH_M = 2.48f;
constexpr float BOXVAN_DOOR_HEIGHT_M = 2.70f;
}
