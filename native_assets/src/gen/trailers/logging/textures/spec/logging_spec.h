#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::trailers::logging {
constexpr uint32_t LOGGING_TEX_WIDTH = 1024;
constexpr uint32_t LOGGING_TEX_HEIGHT = 1024;
constexpr VkFormat LOGGING_ALBEDO_FORMAT = VK_FORMAT_BC7_SRGB_BLOCK;
constexpr VkFormat LOGGING_NORMAL_FORMAT = VK_FORMAT_BC5_UNORM_BLOCK;
constexpr VkFormat LOGGING_ORM_FORMAT = VK_FORMAT_BC7_UNORM_BLOCK;
constexpr float LOGGING_MAX_LOG_LENGTH_M = 16.0f;
constexpr float LOGGING_BUNK_SPACING_M = 3.20f;
}
