#include "VulkanTexture.hpp"
#include <fstream>
#include <cstring>
#include <vector>
#include <cmath>
#include <stdexcept>
namespace sphere::base {
namespace {
uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return 0;
}
} // namespace
bool VulkanTexture::LoadKTX2FromFile(
    const std::string& filename,
    VkDevice inDevice,
    VkPhysicalDevice physicalDevice,
    VkQueue copyQueue,
    VkCommandPool copyPool,
    VkFormat formatOverride
) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return false;
    }
    size_t fileSize = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        return false;
    }
    file.close();
    return LoadKTX2FromMemory(buffer.data(), fileSize, inDevice, physicalDevice, copyQueue, copyPool, formatOverride);
}
bool VulkanTexture::LoadKTX2FromMemory(
    const void* data,
    size_t dataSize,
    VkDevice inDevice,
    VkPhysicalDevice physicalDevice,
    VkQueue copyQueue,
    VkCommandPool copyPool,
    VkFormat formatOverride
) {
    if (dataSize < sizeof(Ktx2Header)) {
        return false;
    }
    const uint8_t* rawBytes = reinterpret_cast<const uint8_t*>(data);
    const Ktx2Header* header = reinterpret_cast<const Ktx2Header*>(rawBytes);
    // Verify KTX2 identifier
    const uint8_t ktx2Magic[12] = {0xAB, 0x4B, 0x54, 0x58, 0x20, 0x32, 0x30, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A};
    if (std::memcmp(header->identifier, ktx2Magic, 12) != 0) {
        return false;
    }
    device = inDevice;
    width = header->pixelWidth;
    height = header->pixelHeight;
    mipLevels = (header->levelCount > 0) ? header->levelCount : 1;
    layerCount = (header->layerCount > 0) ? header->layerCount : 1;
    format = (formatOverride != VK_FORMAT_UNDEFINED) ? formatOverride : static_cast<VkFormat>(header->vkFormat);
    if (format == VK_FORMAT_UNDEFINED) {
        format = VK_FORMAT_R8G8B8A8_SRGB;
    }
    // Read level index entries
    const Ktx2LevelIndex* levelIndices = reinterpret_cast<const Ktx2LevelIndex*>(rawBytes + sizeof(Ktx2Header));
    // Calculate staging size and collect mip copy regions
    std::vector<VkBufferImageCopy> copyRegions(mipLevels);
    VkDeviceSize totalStagingSize = 0;
    std::vector<VkDeviceSize> stagingOffsets(mipLevels);
    for (uint32_t i = 0; i < mipLevels; i++) {
        uint64_t levelLen = levelIndices[i].byteLength;
        stagingOffsets[i] = totalStagingSize;
        totalStagingSize += levelLen;
        uint32_t mipW = std::max(1u, width >> i);
        uint32_t mipH = std::max(1u, height >> i);
        copyRegions[i].bufferOffset = stagingOffsets[i];
        copyRegions[i].bufferRowLength = 0;
        copyRegions[i].bufferImageHeight = 0;
        copyRegions[i].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegions[i].imageSubresource.mipLevel = i;
        copyRegions[i].imageSubresource.baseArrayLayer = 0;
        copyRegions[i].imageSubresource.layerCount = 1;
        copyRegions[i].imageOffset = {0, 0, 0};
        copyRegions[i].imageExtent = {mipW, mipH, 1};
    }
    // Create host-visible staging buffer
    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingMemory = VK_NULL_HANDLE;
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = totalStagingSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer) != VK_SUCCESS) {
        return false;
    }
    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(device, stagingBuffer, &memReqs);
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = FindMemoryType(
        physicalDevice,
        memReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    if (vkAllocateMemory(device, &allocInfo, nullptr, &stagingMemory) != VK_SUCCESS) {
        vkDestroyBuffer(device, stagingBuffer, nullptr);
        return false;
    }
    vkBindBufferMemory(device, stagingBuffer, stagingMemory, 0);
    // Copy mip levels into staging buffer
    void* mappedPtr = nullptr;
    vkMapMemory(device, stagingMemory, 0, totalStagingSize, 0, &mappedPtr);
    for (uint32_t i = 0; i < mipLevels; i++) {
        uint64_t fileOffset = levelIndices[i].byteOffset;
        uint64_t levelLen = levelIndices[i].byteLength;
        std::memcpy(
            reinterpret_cast<uint8_t*>(mappedPtr) + stagingOffsets[i],
            rawBytes + fileOffset,
            levelLen
        );
    }
    vkUnmapMemory(device, stagingMemory);
    // Create device-local destination VkImage
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingMemory, nullptr);
        return false;
    }
    vkGetImageMemoryRequirements(device, image, &memReqs);
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = FindMemoryType(
        physicalDevice,
        memReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    if (vkAllocateMemory(device, &allocInfo, nullptr, &deviceMemory) != VK_SUCCESS) {
        vkDestroyImage(device, image, nullptr);
        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingMemory, nullptr);
        return false;
    }
    vkBindImageMemory(device, image, deviceMemory, 0);
    // Allocate transient command buffer for upload & transitions
    VkCommandBufferAllocateInfo cmdAllocInfo{};
    cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdAllocInfo.commandPool = copyPool;
    cmdAllocInfo.commandBufferCount = 1;
    VkCommandBuffer copyCmd;
    vkAllocateCommandBuffers(device, &cmdAllocInfo, &copyCmd);
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(copyCmd, &beginInfo);
    // Barrier 1: Transition UNDEFINED -> TRANSFER_DST_OPTIMAL
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    vkCmdPipelineBarrier(
        copyCmd,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
    // Copy staging buffer mips into image
    vkCmdCopyBufferToImage(
        copyCmd,
        stagingBuffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        static_cast<uint32_t>(copyRegions.size()),
        copyRegions.data()
    );
    // Barrier 2: Transition TRANSFER_DST_OPTIMAL -> SHADER_READ_ONLY_OPTIMAL
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    vkCmdPipelineBarrier(
        copyCmd,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
    vkEndCommandBuffer(copyCmd);
    // Submit and wait
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &copyCmd;
    vkQueueSubmit(copyQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(copyQueue);
    vkFreeCommandBuffers(device, copyPool, 1, &copyCmd);
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingMemory, nullptr);
    imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    // Create image view
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    if (vkCreateImageView(device, &viewInfo, nullptr, &view) != VK_SUCCESS) {
        Destroy();
        return false;
    }
    // Create sampler with trilinear filtering & aniso support
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.maxAnisotropy = 8.0f;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(mipLevels);
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
        Destroy();
        return false;
    }
    UpdateDescriptor();
    return true;
}
void VulkanTexture::UpdateDescriptor() {
    descriptor.sampler = sampler;
    descriptor.imageView = view;
    descriptor.imageLayout = imageLayout;
}
void VulkanTexture::Destroy() {
    if (sampler) {
        vkDestroySampler(device, sampler, nullptr);
        sampler = VK_NULL_HANDLE;
    }
    if (view) {
        vkDestroyImageView(device, view, nullptr);
        view = VK_NULL_HANDLE;
    }
    if (image) {
        vkDestroyImage(device, image, nullptr);
        image = VK_NULL_HANDLE;
    }
    if (deviceMemory) {
        vkFreeMemory(device, deviceMemory, nullptr);
        deviceMemory = VK_NULL_HANDLE;
    }
}
} // namespace sphere::base