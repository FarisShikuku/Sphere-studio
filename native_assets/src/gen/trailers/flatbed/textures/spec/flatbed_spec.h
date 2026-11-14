#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>
namespace sphere::trailers::flatbed {
constexpr uint32_t FLATBED_TEX_WIDTH = 1024;
constexpr uint32_t FLATBED_TEX_HEIGHT = 1024;
constexpr VkFormat FLATBED_ALBEDO_FORMAT = VK_FORMAT_BC7_SRGB_BLOCK;
constexpr VkFormat FLATBED_NORMAL_FORMAT = VK_FORMAT_BC5_UNORM_BLOCK;
constexpr VkFormat FLATBED_ORM_FORMAT = VK_FORMAT_BC7_UNORM_BLOCK;
}
