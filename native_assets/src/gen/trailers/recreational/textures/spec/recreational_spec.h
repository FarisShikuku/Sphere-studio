#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::trailers::recreational {
constexpr uint32_t RECREATION_TEX_WIDTH = 1024;
constexpr uint32_t RECREATION_TEX_HEIGHT = 1024;
constexpr VkFormat RECREATION_ALBEDO_FORMAT = VK_FORMAT_BC7_SRGB_BLOCK;
constexpr VkFormat RECREATION_NORMAL_FORMAT = VK_FORMAT_BC5_UNORM_BLOCK;
constexpr VkFormat RECREATION_ORM_FORMAT = VK_FORMAT_BC7_UNORM_BLOCK;
constexpr float RECREATION_BALL_SIZE_MM = 50.0f;
constexpr float RECREATION_MAX_GVW_KG = 3500.0f;
}
