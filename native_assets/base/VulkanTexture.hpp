#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <cstdint>
namespace sphere::base {
// Standard Khronos Texture 2.0 (KTX2) header definition
// Reference: Khronos KTX 2.0 Specification & SaschaWillems/Vulkan texture loading
#pragma pack(push, 1)
struct Ktx2Header {
    uint8_t identifier[12];
    uint32_t vkFormat;
    uint32_t typeSize;
    uint32_t pixelWidth;
    uint32_t pixelHeight;
    uint32_t pixelDepth;
    uint32_t layerCount;
    uint32_t faceCount;
    uint32_t levelCount;
    uint32_t supercompressionScheme;
    uint32_t dfdByteOffset;
    uint32_t dfdByteLength;
    uint32_t kvdByteOffset;
    uint32_t kvdByteLength;
    uint64_t sgdByteOffset;
    uint64_t sgdByteLength;
};
struct Ktx2LevelIndex {
    uint64_t byteOffset;
    uint64_t byteLength;
    uint64_t uncompressedByteLength;
};
#pragma pack(pop)
class VulkanTexture {
public:
    VkDevice device = VK_NULL_HANDLE;
    VkImage image = VK_NULL_HANDLE;
    VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t mipLevels = 1;
    uint32_t layerCount = 1;
    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    VkDescriptorImageInfo descriptor{};
    VkSampler sampler = VK_NULL_HANDLE;
    /**
     * Loads a KTX2 texture from disk, creates device-local VkImage, uploads all mip levels
     * via a staging buffer, transitions layout to SHADER_READ_ONLY_OPTIMAL, and creates sampler + view.
     */
    bool LoadKTX2FromFile(
        const std::string& filename,
        VkDevice inDevice,
        VkPhysicalDevice physicalDevice,
        VkQueue copyQueue,
        VkCommandPool copyPool,
        VkFormat formatOverride = VK_FORMAT_UNDEFINED
    );
    /**
     * Loads a KTX2 texture from memory buffer.
     */
    bool LoadKTX2FromMemory(
        const void* data,
        size_t size,
        VkDevice inDevice,
        VkPhysicalDevice physicalDevice,
        VkQueue copyQueue,
        VkCommandPool copyPool,
        VkFormat formatOverride = VK_FORMAT_UNDEFINED
    );
    void UpdateDescriptor();
    void Destroy();
};
} // namespace sphere::base