#include "VulkanTextPipeline.h"
#include <cstring>
#include <algorithm>

namespace sphere {

VulkanTextPipeline::VulkanTextPipeline()
    : m_vertexCount(0), m_stagingBufferSize(65536), m_isInitialized(false) {
}

VulkanTextPipeline::~VulkanTextPipeline() {
    // Release Vulkan buffers, pipelines, descriptor sets
}

bool VulkanTextPipeline::preparePipeline(uint32_t /*sampleCount*/) {
    // Pipeline creation logic following Sascha Willems distancefieldfonts:
    // 1. Setup descriptor pool & layout (CombinedImageSampler for Font Atlas)
    // 2. Setup push constant range (sizeof(TextPushConstants) = 144 bytes, well within 128-256 byte limits)
    // 3. Setup graphics pipeline with:
    //    - Vertex input: Location 0: vec3 pos, Location 1: vec2 uv, Location 2: vec4 color
    //    - Rasterizer: VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE (for double-sided 3D billboards)
    //    - Blend state: VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD
    //    - Depth test: Optional depth test with VK_COMPARE_OP_LESS_OR_EQUAL for 3D world text; disabled for 2D HUD.
    m_isInitialized = true;
    return true;
}

bool VulkanTextPipeline::updateVertexBuffer(const std::vector<TextVertex>& vertices) {
    m_vertexCount = static_cast<uint32_t>(vertices.size());
    size_t requiredSize = vertices.size() * sizeof(TextVertex);

    if (requiredSize > m_stagingBufferSize) {
        m_stagingBufferSize = std::max(requiredSize * 2, m_stagingBufferSize * 2);
    }

    // Host-mapped staging buffer memcpy to device-local vertex buffer
    // void* data;
    // vkMapMemory(device, stagingBufferMemory, 0, requiredSize, 0, &data);
    // memcpy(data, vertices.data(), requiredSize);
    // vkUnmapMemory(device, stagingBufferMemory);
    // VkBufferCopy copyRegion = {0, 0, requiredSize};
    // vkCmdCopyBuffer(cmd, stagingBuffer, vertexBuffer, 1, &copyRegion);

    return true;
}

void VulkanTextPipeline::recordDrawCommands(
    void* /*commandBufferHandle*/,
    const TextDef& textDef,
    const float* mvpMatrix
) {
    if (!m_isInitialized || m_vertexCount == 0) return;

    TextPushConstants pushConstants{};
    if (mvpMatrix) {
        std::memcpy(pushConstants.projectionMatrix, mvpMatrix, 16 * sizeof(float));
    }

    std::memcpy(pushConstants.textColor, textDef.sdfParams.textColor.data(), 4 * sizeof(float));
    std::memcpy(pushConstants.outlineColor, textDef.sdfParams.outlineColor.data(), 4 * sizeof(float));
    std::memcpy(pushConstants.shadowColor, textDef.sdfParams.shadowColor.data(), 4 * sizeof(float));
    std::memcpy(pushConstants.shadowOffset, textDef.sdfParams.shadowOffset.data(), 2 * sizeof(float));

    pushConstants.outlineWidth = textDef.sdfParams.outlineWidth;
    pushConstants.outlineEdge = 0.5f - textDef.sdfParams.thicknessBias;
    pushConstants.smoothing = textDef.sdfParams.smoothing;
    pushConstants.thicknessBias = textDef.sdfParams.thicknessBias;
    pushConstants.glowRadius = textDef.sdfParams.glowRadius;
    std::memcpy(pushConstants.glowColor, textDef.sdfParams.glowColor.data(), 4 * sizeof(float));

    uint32_t flags = 0;
    if (textDef.sdfParams.enableOutline) flags |= (1 << 0);
    if (textDef.sdfParams.enableDropShadow) flags |= (1 << 1);
    if (textDef.sdfParams.enableGlow) flags |= (1 << 2);
    if (textDef.renderMode == TextRenderMode::BILLBOARD_3D_WORLD) flags |= (1 << 3);
    pushConstants.renderFlags = flags;

    // Command buffer recording:
    // vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    // vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    // vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(pushConstants), &pushConstants);
    // VkDeviceSize offsets[1] = {0};
    // vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, offsets);
    // vkCmdDraw(cmd, m_vertexCount, 1, 0, 0);
}

} // namespace sphere
