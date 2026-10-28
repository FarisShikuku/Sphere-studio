#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

namespace sphere {

struct WaterSurfaceTextureSpec {
    static constexpr uint32_t Resolution = 2048;
    static constexpr float WaterIor = 1.333f;

    static constexpr VkFormat AlbedoFormat = VK_FORMAT_BC7_UNORM_SRGB_BLOCK;
    static constexpr VkFormat NormalFormat = VK_FORMAT_BC5_UNORM_BLOCK;
    static constexpr VkFormat CausticFormat = VK_FORMAT_R8_UNORM;
};

} // namespace sphere
