#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>
namespace sphere {
struct GrassSurfaceTextureSpec {
    static constexpr uint32_t Resolution = 2048;
    static constexpr float TileSizeMeters = 0.30f; // 300mm mondo grass patch
    static constexpr VkFormat AlbedoFormat = VK_FORMAT_BC7_SRGB_BLOCK;
    static constexpr VkFormat NormalFormat = VK_FORMAT_BC5_UNORM_BLOCK;
    static constexpr VkFormat OrmFormat    = VK_FORMAT_BC7_UNORM_BLOCK;
};
} // namespace spher
