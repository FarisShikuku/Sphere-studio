#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere::trailers::heavy_haulage {
constexpr uint32_t HEAVY_TEX_WIDTH = 1024;
constexpr uint32_t HEAVY_TEX_HEIGHT = 1024;
constexpr VkFormat HEAVY_ALBEDO_FORMAT = VK_FORMAT_BC7_SRGB_BLOCK;
constexpr VkFormat HEAVY_NORMAL_FORMAT = VK_FORMAT_BC5_UNORM_BLOCK;
constexpr VkFormat HEAVY_ORM_FORMAT = VK_FORMAT_BC7_UNORM_BLOCK;
constexpr float HEAVY_MAX_LOAD_TONS = 180.0f;
constexpr float HEAVY_WELL_DECK_HEIGHT_M = 0.58f;
}
