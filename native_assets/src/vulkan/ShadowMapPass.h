#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <cstdint>

namespace sphere::vulkan {

/**
 * @brief Vulkan 1.3 Offscreen Shadow Map Pass
 * 
 * Direct reference architecture from:
 * https://github.com/SaschaWillems/Vulkan/tree/master/examples/shadowmapping
 * 
 * Features:
 * - Dedicated offscreen depth render pass / dynamic rendering
 * - VK_FORMAT_D32_SFLOAT depth buffer attachment
 * - Sampler with VK_COMPARE_OP_LESS_OR_EQUAL and clamp-to-edge
 * - Depth bias dynamic state (constant factor & slope factor) to eliminate shadow acne
 * - Orthographic light-space projection matrix calculation for directional sunlight
 */
class ShadowMapPass {
public:
    enum class FilterQuality {
        Hard = 0,    // 1-Tap raw depth comparison
        PCF_3x3 = 1, // 3x3 Percentage Closer Filtering (9 taps)
        PCF_5x5 = 2, // 5x5 Percentage Closer Filtering (25 taps - Sascha Willems standard)
        CSM_4 = 3    // 4-Cascade Cascaded Shadow Maps
    };

    struct Settings {
        uint32_t resolution = 2048;
        float depthBiasConstant = 1.25f;
        float depthBiasSlope = 1.75f;
        float pcfFilterRadius = 1.5f;
        FilterQuality quality = FilterQuality::PCF_5x5;
        bool enabled = true;
    };

    ShadowMapPass() = default;
    ~ShadowMapPass();

    bool init(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t resolution = 2048);
    void cleanup();

    // Updates directional sun view and orthographic projection matrices
    void updateLightMatrices(const glm::vec3& sunDir, const glm::vec3& sceneCenter, float sceneRadius);

    void beginPass(VkCommandBuffer cmd);
    void endPass(VkCommandBuffer cmd);

    // Getters for scene binding
    VkDescriptorImageInfo getDescriptorImageInfo() const;
    const glm::mat4& getLightSpaceMatrix() const { return m_lightSpaceMatrix; }
    const glm::mat4& getLightView() const { return m_lightView; }
    const glm::mat4& getLightProj() const { return m_lightProj; }
    const Settings& getSettings() const { return m_settings; }
    Settings& getSettings() { return m_settings; }

private:
    VkDevice m_device{VK_NULL_HANDLE};
    uint32_t m_resolution{2048};
    Settings m_settings;

    VkImage m_depthImage{VK_NULL_HANDLE};
    VkDeviceMemory m_depthMemory{VK_NULL_HANDLE};
    VkImageView m_depthView{VK_NULL_HANDLE};
    VkSampler m_depthSampler{VK_NULL_HANDLE};
    VkFramebuffer m_framebuffer{VK_NULL_HANDLE};
    VkRenderPass m_renderPass{VK_NULL_HANDLE};

    glm::mat4 m_lightView{1.0f};
    glm::mat4 m_lightProj{1.0f};
    glm::mat4 m_lightSpaceMatrix{1.0f};
};

} // namespace sphere::vulkan
