#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>
namespace sphere {
struct BrickTextureSpec {
    static constexpr uint32_t Resolution = 2048;
    static constexpr float TileSizeMeters = 0.90f; // 900mm stretcher bond
    static constexpr float BrickWidthMm = 215.0f;
    static constexpr float BrickHeightMm = 65.0f;
    static constexpr float MortarWidthMm = 10.0f;
    // Vulkan 1.3 Image Formats (from Textures surface.md)
    static constexpr VkFormat AlbedoFormat = VK_FORMAT_BC7_SRGB_BLOCK;
    static constexpr VkFormat NormalFormat = VK_FORMAT_BC5_UNORM_BLOCK;
    static constexpr VkFormat OrmFormat    = VK_FORMAT_BC7_UNORM_BLOCK;
    static constexpr VkFormat HeightFormat = VK_FORMAT_BC4_UNORM_BLOCK;
};
} // namespace spher
