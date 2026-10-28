#pragma once
#include "TextDef.h"
#include <string>
#include <vector>

namespace sphere {

/**
 * Push constants structure matching distancefieldfonts.vert and .frag.
 */
struct TextPushConstants {
    float projectionMatrix[16];   // 4x4 Ortho or Perspective MVP matrix
    float textColor[4];           // RGBA primary font color
    float outlineColor[4];        // RGBA stroke outline color
    float shadowColor[4];         // RGBA drop shadow color
    float shadowOffset[2];        // Drop shadow UV shift
    float outlineWidth;           // SDF outline threshold offset
    float outlineEdge;            // 0.5 center threshold
    float smoothing;              // Antialiasing smoothstep delta
    float thicknessBias;          // Font dilation bias
    float glowRadius;             // Emissive glow spread
    float glowColor[4];           // Emissive glow RGBA
    uint32_t renderFlags;         // Bit 0: Outline, Bit 1: Shadow, Bit 2: Glow, Bit 3: 3D Billboard
};

/**
 * VulkanTextPipeline manages GPU pipelines, descriptors, vertex staging buffers,
 * and command recording for high-performance text and SDF rendering in Vulkan.
 * Implements the architecture from Sascha Willems Vulkan textoverlay & distancefieldfonts.
 */
class VulkanTextPipeline {
public:
    VulkanTextPipeline();
    ~VulkanTextPipeline();

    // Prepare pipeline layout, descriptor set layouts, and PSO
    bool preparePipeline(uint32_t sampleCount = 1);

    // Staging and upload of text mesh vertices to GPU
    bool updateVertexBuffer(const std::vector<TextVertex>& vertices);

    // Record draw commands into active Vulkan command buffer
    void recordDrawCommands(
        void* commandBufferHandle,
        const TextDef& textDef,
        const float* mvpMatrix
    );

    // Pipeline metrics & diagnostics
    uint32_t getVertexCount() const { return m_vertexCount; }
    uint32_t getTriangleCount() const { return m_vertexCount / 3; }
    size_t getStagingBufferSize() const { return m_stagingBufferSize; }
    bool isSdfEnabled() const { return true; }

private:
    uint32_t m_vertexCount = 0;
    size_t m_stagingBufferSize = 65536; // 64 KB initial staging allocation
    bool m_isInitialized = false;
};

} // namespace sphere
