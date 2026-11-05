#pragma once
#include "LightingSystem.h"
#include "Atmosphere.h"
#include "SdfGiSystem.h"
#include "CascadedShadows.h"
#include "TaaPipeline.h"
#include <string>
#include <vector>

namespace sphere::lighting {

/**
 * VulkanLightPipeline - Master PlainRenderer Render Pass Coordinator
 *
 * Coordinates the full sequence of Vulkan render passes and compute dispatches:
 * 1. Pass 1: Cascaded Shadow Map Pass (VkRenderPass - Depth only, 4 cascades)
 * 2. Pass 2: Deferred G-Buffer Pass (VkRenderPass - MRT Albedo, Normal, Material, Velocity)
 * 3. Pass 3: SDF Diffuse GI Compute Pass (vkCmdDispatch - 3D Volume raymarch)
 * 4. Pass 4: Physical Atmosphere & Skybox Pass (VkRenderPass - Preetham / Bruneton sky)
 * 5. Pass 5: Deferred PBR Lighting Resolve (VkRenderPass - Direct + Shadows + SDF GI + Sky)
 * 6. Pass 6: Temporal Anti-Aliasing (TAA) Resolve Pass (VkRenderPass - Halton subpixel jitter)
 */

enum class RenderPipelineStage {
    SHADOW_PASS,
    GBUFFER_PASS,
    SDF_GI_COMPUTE_PASS,
    ATMOSPHERE_SKY_PASS,
    DEFERRED_LIGHTING_PASS,
    TAA_RESOLVE_PASS
};

enum class PipelineDebugView {
    FINAL_COMBINED = 0,
    DIRECT_LIGHT_ONLY = 1,
    SDF_GI_INDIRECT_ONLY = 2,
    DISTANCE_FIELD_VOXELS = 3,
    SHADOW_CASCADES_DEBUG = 4,
    ATMOSPHERIC_SKY_ONLY = 5,
    AMBIENT_OCCLUSION_DFAO = 6
};

struct PipelineStatistics {
    uint32_t activeLightsCount = 1;
    uint32_t shadowCascadeSplits = 4;
    uint32_t sdfVoxelResolution = 64;
    uint32_t sdfRaymarchStepsMax = 32;
    float directLightMs = 0.42f;
    float sdfGiComputeMs = 1.15f;
    float shadowPassMs = 0.35f;
    float atmosphereSkyMs = 0.22f;
    float taaResolveMs = 0.28f;
    float totalFrameMs = 2.42f;
};

class VulkanLightPipeline {
public:
    VulkanLightPipeline();
    ~VulkanLightPipeline() = default;

    // Subsystem accessors
    LightingSystem& GetLightingSystem() { return m_lightingSystem; }
    const LightingSystem& GetLightingSystem() const { return m_lightingSystem; }

    Atmosphere& GetAtmosphere() { return m_atmosphere; }
    const Atmosphere& GetAtmosphere() const { return m_atmosphere; }

    SdfGiSystem& GetSdfGi() { return m_sdfGi; }
    const SdfGiSystem& GetSdfGi() const { return m_sdfGi; }

    CascadedShadows& GetCascadedShadows() { return m_cascadedShadows; }
    const CascadedShadows& GetCascadedShadows() const { return m_cascadedShadows; }

    TaaPipeline& GetTaa() { return m_taa; }
    const TaaPipeline& GetTaa() const { return m_taa; }

    // Debug view mode selection
    void SetDebugView(PipelineDebugView view) { m_debugView = view; }
    PipelineDebugView GetDebugView() const { return m_debugView; }

    // Multi-pass execution simulator for statistics and state tracking
    void ExecuteFrame(uint32_t width, uint32_t height);

    PipelineStatistics GetStatistics() const { return m_stats; }

    // Generates human-readable pipeline description
    std::string GetPipelineStatusSummary() const;

private:
    LightingSystem m_lightingSystem;
    Atmosphere m_atmosphere;
    SdfGiSystem m_sdfGi;
    CascadedShadows m_cascadedShadows;
    TaaPipeline m_taa;

    PipelineDebugView m_debugView = PipelineDebugView::FINAL_COMBINED;
    PipelineStatistics m_stats;
};

} // namespace sphere::lighting
