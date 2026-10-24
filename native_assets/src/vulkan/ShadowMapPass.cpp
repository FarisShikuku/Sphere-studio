#include "ShadowMapPass.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace sphere::vulkan {

ShadowMapPass::~ShadowMapPass() {
    cleanup();
}

bool ShadowMapPass::init(VkDevice device, VkPhysicalDevice /*physicalDevice*/, uint32_t resolution) {
    m_device = device;
    m_resolution = resolution;
    m_settings.resolution = resolution;

    // In a complete Vulkan runtime:
    // 1. Create depth VkImage with VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
    // 2. Allocate and bind device memory
    // 3. Create VkImageView (VK_IMAGE_ASPECT_DEPTH_BIT)
    // 4. Create VkSampler with VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER (borderColor = 1.0f)
    //    and VK_COMPARE_OP_LESS_OR_EQUAL for hardware shadow comparison.
    // 5. Create VkRenderPass with depth-only attachment (loadOp = CLEAR, storeOp = STORE)
    // 6. Create VkFramebuffer wrapping depthView at (resolution x resolution)

    return true;
}

void ShadowMapPass::cleanup() {
    if (m_device != VK_NULL_HANDLE) {
        if (m_depthSampler != VK_NULL_HANDLE) {
            vkDestroySampler(m_device, m_depthSampler, nullptr);
            m_depthSampler = VK_NULL_HANDLE;
        }
        if (m_depthView != VK_NULL_HANDLE) {
            vkDestroyImageView(m_device, m_depthView, nullptr);
            m_depthView = VK_NULL_HANDLE;
        }
        if (m_framebuffer != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
            m_framebuffer = VK_NULL_HANDLE;
        }
        if (m_renderPass != VK_NULL_HANDLE) {
            vkDestroyRenderPass(m_device, m_renderPass, nullptr);
            m_renderPass = VK_NULL_HANDLE;
        }
        if (m_depthImage != VK_NULL_HANDLE) {
            vkDestroyImage(m_device, m_depthImage, nullptr);
            m_depthImage = VK_NULL_HANDLE;
        }
        if (m_depthMemory != VK_NULL_HANDLE) {
            vkFreeMemory(m_device, m_depthMemory, nullptr);
            m_depthMemory = VK_NULL_HANDLE;
        }
    }
}

void ShadowMapPass::updateLightMatrices(const glm::vec3& sunDir, const glm::vec3& sceneCenter, float sceneRadius) {
    // Light travels in opposite direction of sunDir
    glm::vec3 normLight = glm::normalize(sunDir);
    glm::vec3 lightPos = sceneCenter + normLight * (sceneRadius * 2.0f);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    if (std::abs(glm::dot(normLight, up)) > 0.99f) {
        up = glm::vec3(0.0f, 0.0f, 1.0f);
    }

    m_lightView = glm::lookAt(lightPos, sceneCenter, up);

    // Tight orthographic projection bounding the scene radius
    float r = sceneRadius * 1.2f;
    m_lightProj = glm::ortho(-r, r, -r, r, 0.1f, sceneRadius * 4.0f);
    // Vulkan clip space Y inverted
    m_lightProj[1][1] *= -1.0f;

    m_lightSpaceMatrix = m_lightProj * m_lightView;
}

void ShadowMapPass::beginPass(VkCommandBuffer cmd) {
    if (cmd == VK_NULL_HANDLE) return;

    // Set dynamic depth bias to eliminate shadow acne (Peter Panning prevention)
    // Reference: SaschaWillems/Vulkan shadowmapping/shadowmapping.cpp
    vkCmdSetDepthBias(
        cmd,
        m_settings.depthBiasConstant,
        0.0f,
        m_settings.depthBiasSlope
    );

    // If using classic renderpass:
    // VkRenderPassBeginInfo beginInfo{...};
    // vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void ShadowMapPass::endPass(VkCommandBuffer cmd) {
    if (cmd == VK_NULL_HANDLE) return;
    // vkCmdEndRenderPass(cmd);
}

VkDescriptorImageInfo ShadowMapPass::getDescriptorImageInfo() const {
    VkDescriptorImageInfo info{};
    info.sampler = m_depthSampler;
    info.imageView = m_depthView;
    info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    return info;
}

} // namespace sphere::vulkan
