#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::trailers::dolly {
constexpr uint32_t DOLLY_TEX_WIDTH = 1024;
constexpr uint32_t DOLLY_TEX_HEIGHT = 1024;
constexpr VkFormat DOLLY_ALBEDO_FORMAT = VK_FORMAT_BC7_SRGB_BLOCK;
constexpr VkFormat DOLLY_NORMAL_FORMAT = VK_FORMAT_BC5_UNORM_BLOCK;
constexpr VkFormat DOLLY_ORM_FORMAT = VK_FORMAT_BC7_UNORM_BLOCK;
constexpr float DOLLY_DRAWBAR_LENGTH_M = 2.40f;
constexpr float DOLLY_HITCH_HEIGHT_M = 0.85f;
}
