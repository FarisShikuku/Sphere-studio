#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>
namespace sphere {
struct GlassTextureSpec {
    static constexpr uint32_t Resolution = 1024;
    static constexpr float DefaultIor = 1.520f; // Crown Glass
    static constexpr VkFormat RefractFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    static constexpr VkFormat DirtFormat    = VK_FORMAT_R8_UNORM;
};
} // namespace spher
