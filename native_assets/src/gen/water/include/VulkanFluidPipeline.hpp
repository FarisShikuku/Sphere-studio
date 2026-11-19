#pragma once

#include "FluidParams.hpp"
#include "vulkan/FrameGraph.h"
#include <vulkan/vulkan.h>
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace sphere::water {

struct VulkanShaderStageSpec {
    const char* name;
    const char* relativePath;
    const char* stageLabel;
    const char* description;
};

/**
 * VulkanFluidPipeline
 *
 * Vulkan 1.3 Compute & Graphics Pipeline integration for the SPH fluid solver
 * and Screen-Space Fluid Rendering (SSFR) passes.
 *
 * Replaces OpenGL 4.6 SSBOs, glDispatchCompute, and FBO attachments with:
 *  - Vulkan Storage Buffers (VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) for particle state
 *  - Descriptor Set Layouts (Set 0 Compute SSBOs, Set 1 Graphics Samplers + UBO)
 *  - Push Constants & Memory Barriers (VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT -> VERTEX/FRAGMENT)
 *  - FrameGraph compute + graphics pass orchestration
 */
class VulkanFluidPipeline {
public:
    VulkanFluidPipeline() = default;
    ~VulkanFluidPipeline();

    bool Initialize(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t maxParticles);
    void Shutdown();

    bool IsDeviceReady() const { return m_DeviceReady; }
    VkDescriptorSetLayout GetComputeDescriptorLayout() const { return m_ComputeDescLayout; }
    VkDescriptorSetLayout GetGraphicsDescriptorLayout() const { return m_GraphicsDescLayout; }
    VkPipelineLayout GetComputePipelineLayout() const { return m_ComputePipelineLayout; }
    VkPipelineLayout GetGraphicsPipelineLayout() const { return m_GraphicsPipelineLayout; }

    void RecordComputeDispatch(
        VkCommandBuffer cmd,
        uint32_t particleCount,
        const GpuFluidUniforms& uniforms
    );

    void RegisterPasses(sphere::vk::FrameGraph& frameGraph, uint32_t particleCount, const GpuFluidUniforms& uniforms);

    static const std::vector<VulkanShaderStageSpec>& GetShaderStageManifest();

private:
    bool CreateDescriptorSetLayouts();
    bool CreatePipelineLayouts();

    VkDevice m_Device = VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    bool m_DeviceReady = false;
    uint32_t m_MaxParticles = 0;

    VkDescriptorSetLayout m_ComputeDescLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_GraphicsDescLayout = VK_NULL_HANDLE;
    VkPipelineLayout m_ComputePipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayout m_GraphicsPipelineLayout = VK_NULL_HANDLE;
};

} // namespace sphere::water
