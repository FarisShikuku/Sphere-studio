#include "HiZ.h"
#include <algorithm>

namespace sphere {

void HiZ::BuildMipChain(VkCommandBuffer cmd, VkImage depthImage, uint32_t width, uint32_t height, uint32_t mipLevels) {
    if (cmd == VK_NULL_HANDLE || depthImage == VK_NULL_HANDLE || mipLevels <= 1) {
        return;
    }

    int32_t mipWidth = static_cast<int32_t>(width);
    int32_t mipHeight = static_cast<int32_t>(height);

    for (uint32_t i = 1; i < mipLevels; ++i) {
        // 1. Transition previous mip level (i - 1) from general or write to transfer/shader read
        VkImageMemoryBarrier barrierPrev{};
        barrierPrev.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrierPrev.image = depthImage;
        barrierPrev.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierPrev.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierPrev.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrierPrev.subresourceRange.baseMipLevel = i - 1;
        barrierPrev.subresourceRange.levelCount = 1;
        barrierPrev.subresourceRange.baseArrayLayer = 0;
        barrierPrev.subresourceRange.layerCount = 1;
        barrierPrev.oldLayout = (i == 1) ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_GENERAL;
        barrierPrev.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrierPrev.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        barrierPrev.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        // 2. Transition current mip level (i) to transfer dst optimal for writing
        VkImageMemoryBarrier barrierCur{};
        barrierCur.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrierCur.image = depthImage;
        barrierCur.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierCur.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierCur.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrierCur.subresourceRange.baseMipLevel = i;
        barrierCur.subresourceRange.levelCount = 1;
        barrierCur.subresourceRange.baseArrayLayer = 0;
        barrierCur.subresourceRange.layerCount = 1;
        barrierCur.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrierCur.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrierCur.srcAccessMask = 0;
        barrierCur.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        VkImageMemoryBarrier barriers[2] = { barrierPrev, barrierCur };
        vkCmdPipelineBarrier(
            cmd,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            2, barriers
        );

        // Blit or downsample command from (i - 1) to (i)
        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;

        int32_t nextWidth = (mipWidth > 1) ? mipWidth / 2 : 1;
        int32_t nextHeight = (mipHeight > 1) ? mipHeight / 2 : 1;

        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { nextWidth, nextHeight, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(
            cmd,
            depthImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            depthImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit,
            VK_FILTER_NEAREST
        );

        // Transition current mip level to General/Shader Read after blit
        barrierCur.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrierCur.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrierCur.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrierCur.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            cmd,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrierCur
        );

        mipWidth = nextWidth;
        mipHeight = nextHeight;
    }

    // Transition all mip levels to SHADER_READ_ONLY_OPTIMAL for culling pass
    VkImageMemoryBarrier finalBarrier{};
    finalBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    finalBarrier.image = depthImage;
    finalBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    finalBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    finalBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    finalBarrier.subresourceRange.baseMipLevel = 0;
    finalBarrier.subresourceRange.levelCount = mipLevels;
    finalBarrier.subresourceRange.baseArrayLayer = 0;
    finalBarrier.subresourceRange.layerCount = 1;
    finalBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    finalBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    finalBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    finalBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
        cmd,
        VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &finalBarrier
    );
}

} // namespace sphere
