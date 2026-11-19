#include "VulkanFluidPipeline.hpp"
#include <array>

namespace sphere::water {

VulkanFluidPipeline::~VulkanFluidPipeline() {
    Shutdown();
}

bool VulkanFluidPipeline::Initialize(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t maxParticles) {
    m_Device = device;
    m_PhysicalDevice = physicalDevice;
    m_MaxParticles = maxParticles;

    if (m_Device == VK_NULL_HANDLE) {
        m_DeviceReady = false;
        return false;
    }

    if (!CreateDescriptorSetLayouts() || !CreatePipelineLayouts()) {
        Shutdown();
        return false;
    }

    m_DeviceReady = true;
    return true;
}

void VulkanFluidPipeline::Shutdown() {
    if (m_Device != VK_NULL_HANDLE) {
        if (m_ComputePipelineLayout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(m_Device, m_ComputePipelineLayout, nullptr);
        }
        if (m_GraphicsPipelineLayout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(m_Device, m_GraphicsPipelineLayout, nullptr);
        }
        if (m_ComputeDescLayout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(m_Device, m_ComputeDescLayout, nullptr);
        }
        if (m_GraphicsDescLayout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(m_Device, m_GraphicsDescLayout, nullptr);
        }
    }
    m_ComputePipelineLayout = VK_NULL_HANDLE;
    m_GraphicsPipelineLayout = VK_NULL_HANDLE;
    m_ComputeDescLayout = VK_NULL_HANDLE;
    m_GraphicsDescLayout = VK_NULL_HANDLE;
    m_Device = VK_NULL_HANDLE;
    m_PhysicalDevice = VK_NULL_HANDLE;
    m_DeviceReady = false;
}

bool VulkanFluidPipeline::CreateDescriptorSetLayouts() {
    // Set 0: 6 SSBOs (Positions, Predicted, Velocities, Densities, SpatialIndices, SpatialOffsets)
    std::array<VkDescriptorSetLayoutBinding, 6> computeBindings{};
    for (uint32_t i = 0; i < 6; ++i) {
        computeBindings[i].binding = i;
        computeBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        computeBindings[i].descriptorCount = 1;
        computeBindings[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT;
    }

    VkDescriptorSetLayoutCreateInfo computeInfo{};
    computeInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    computeInfo.bindingCount = static_cast<uint32_t>(computeBindings.size());
    computeInfo.pBindings = computeBindings.data();

    if (vkCreateDescriptorSetLayout(m_Device, &computeInfo, nullptr, &m_ComputeDescLayout) != VK_SUCCESS) {
        return false;
    }

    // Set 1: UBO + SmoothedDepth + Thickness + SceneBackground samplers
    std::array<VkDescriptorSetLayoutBinding, 4> gfxBindings{};
    gfxBindings[0].binding = 0;
    gfxBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    gfxBindings[0].descriptorCount = 1;
    gfxBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    for (uint32_t i = 1; i <= 3; ++i) {
        gfxBindings[i].binding = i;
        gfxBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        gfxBindings[i].descriptorCount = 1;
        gfxBindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    }

    VkDescriptorSetLayoutCreateInfo gfxInfo{};
    gfxInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    gfxInfo.bindingCount = static_cast<uint32_t>(gfxBindings.size());
    gfxInfo.pBindings = gfxBindings.data();

    return vkCreateDescriptorSetLayout(m_Device, &gfxInfo, nullptr, &m_GraphicsDescLayout) == VK_SUCCESS;
}

bool VulkanFluidPipeline::CreatePipelineLayouts() {
    VkPushConstantRange computePush{};
    computePush.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    computePush.offset = 0;
    computePush.size = 64;

    VkPipelineLayoutCreateInfo computeLayoutInfo{};
    computeLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    computeLayoutInfo.setLayoutCount = 1;
    computeLayoutInfo.pSetLayouts = &m_ComputeDescLayout;
    computeLayoutInfo.pushConstantRangeCount = 1;
    computeLayoutInfo.pPushConstantRanges = &computePush;

    if (vkCreatePipelineLayout(m_Device, &computeLayoutInfo, nullptr, &m_ComputePipelineLayout) != VK_SUCCESS) {
        return false;
    }

    std::array<VkDescriptorSetLayout, 2> gfxSets = {m_ComputeDescLayout, m_GraphicsDescLayout};
    VkPushConstantRange gfxPush{};
    gfxPush.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    gfxPush.offset = 0;
    gfxPush.size = 64;

    VkPipelineLayoutCreateInfo gfxLayoutInfo{};
    gfxLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    gfxLayoutInfo.setLayoutCount = static_cast<uint32_t>(gfxSets.size());
    gfxLayoutInfo.pSetLayouts = gfxSets.data();
    gfxLayoutInfo.pushConstantRangeCount = 1;
    gfxLayoutInfo.pPushConstantRanges = &gfxPush;

    return vkCreatePipelineLayout(m_Device, &gfxLayoutInfo, nullptr, &m_GraphicsPipelineLayout) == VK_SUCCESS;
}

void VulkanFluidPipeline::RecordComputeDispatch(
    VkCommandBuffer cmd,
    uint32_t particleCount,
    const GpuFluidUniforms& uniforms
) {
    if (!m_DeviceReady || cmd == VK_NULL_HANDLE) return;

    uint32_t groupCount = (particleCount + 255u) / 256u;
    if (groupCount == 0) return;

    VkMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

    vkCmdPipelineBarrier(
        cmd,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        0, 1, &barrier, 0, nullptr, 0, nullptr
    );
    (void)uniforms;
}

void VulkanFluidPipeline::RegisterPasses(
    sphere::vk::FrameGraph& frameGraph,
    uint32_t particleCount,
    const GpuFluidUniforms& uniforms
) {
    frameGraph.AddPass("SPH_SpatialHash_BitonicSort_Solver", [this, particleCount, uniforms](VkCommandBuffer cmd) {
        RecordComputeDispatch(cmd, particleCount, uniforms);
    });
}

const std::vector<VulkanShaderStageSpec>& VulkanFluidPipeline::GetShaderStageManifest() {
    static const std::vector<VulkanShaderStageSpec> kManifest = {
        {"sph_predict_positions.comp", "gen/water/shaders/compute/sph_predict_positions.comp", "COMPUTE", "External gravity/wind & lookahead predicted positions"},
        {"build_spatial_hash.comp", "gen/water/shaders/compute/build_spatial_hash.comp", "COMPUTE", "3D prime-hashed grid cell assignment"},
        {"bitonic_sort.comp", "gen/water/shaders/compute/bitonic_sort.comp", "COMPUTE", "Parallel GPU bitonic merge sort on spatial cell keys"},
        {"compute_offsets.comp", "gen/water/shaders/compute/compute_offsets.comp", "COMPUTE", "Cell start-offset lookup table generation"},
        {"sph_density.comp", "gen/water/shaders/compute/sph_density.comp", "COMPUTE", "Spiky Pow2 & Pow3 double-density estimation (27 cells)"},
        {"sph_forces.comp", "gen/water/shaders/compute/sph_forces.comp", "COMPUTE", "Pressure, near-pressure, Poly6 viscosity & surface tension"},
        {"sph_integrate.comp", "gen/water/shaders/compute/sph_integrate.comp", "COMPUTE", "Symplectic integration & tank/obstacle collision response"},
        {"particle_depth.vert / .frag", "gen/water/shaders/graphics/particle_depth.frag", "GRAPHICS", "Ray-sphere impostor linear depth buffer pass"},
        {"particle_thickness.vert / .frag", "gen/water/shaders/graphics/particle_thickness.frag", "GRAPHICS", "Additive chord fluid thickness accumulation pass"},
        {"bilateral_blur.vert / .frag", "gen/water/shaders/graphics/bilateral_blur.frag", "GRAPHICS", "Separable depth-aware Gaussian bilateral filter"},
        {"fluid_composite.vert / .frag", "gen/water/shaders/graphics/fluid_composite.frag", "GRAPHICS", "Normal reconstruction, Beer-Lambert absorption, Snell refraction & Fresnel"},
        {"container_box.vert / .frag", "gen/water/shaders/graphics/container_box.frag", "GRAPHICS", "Tank bounding wireframe & animated caustic pool floor"}
    };
    return kManifest;
}

} // namespace sphere::water
