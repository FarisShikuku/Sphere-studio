#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::trailers::specialty {
constexpr uint32_t SPECIALTY_TEX_WIDTH = 1024;
constexpr uint32_t SPECIALTY_TEX_HEIGHT = 1024;
constexpr VkFormat SPECIALTY_ALBEDO_FORMAT = VK_FORMAT_BC7_SRGB_BLOCK;
constexpr VkFormat SPECIALTY_NORMAL_FORMAT = VK_FORMAT_BC5_UNORM_BLOCK;
constexpr VkFormat SPECIALTY_ORM_FORMAT = VK_FORMAT_BC7_UNORM_BLOCK;
constexpr float SPECIALTY_VENT_SLAT_RATIO = 0.38f;
constexpr float SPECIALTY_ARMOR_PLATE_MM = 12.0f;
}
