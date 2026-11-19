#include "WaterSimulationEngine.hpp"
#include "vulkan/VulkanNativeSceneRenderer.h"
#include <algorithm>
#include <cmath>

namespace sphere::water {

WaterSimulationEngine& WaterSimulationEngine::Get() {
    static WaterSimulationEngine instance;
    return instance;
}

WaterSimulationEngine::WaterSimulationEngine() {
    m_Simulator.Initialize(m_PhysicsParams);
    m_Camera.SetOrbit(-34.0f, 22.0f, 4.6f);
    m_Camera.SetTarget(Vec3(0.0f, 0.85f, 0.0f));
    m_Camera.SetViewport(360, 240, 48.0f);
    m_Initialized = true;
}

bool WaterSimulationEngine::Generate(sphere::GeneratorContext& context, std::shared_ptr<sphere::SceneNode> outNode) {
    (void)context;
    (void)outNode;
    std::lock_guard<std::mutex> lock(m_Mutex);
    if (!m_Initialized) {
        m_Simulator.Initialize(m_PhysicsParams);
        m_Initialized = true;
    }
    return true;
}

void WaterSimulationEngine::Configure(
    const FluidPhysicsParams& physics,
    const FluidRenderParams& render,
    float camYawDeg,
    float camPitchDeg,
    float camDistance
) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    FluidPhysicsParams merged = physics;
    // Preserve waterfall parameters managed via ConfigureWaterfall
    merged.waterfallEnabled = m_PhysicsParams.waterfallEnabled || (physics.scenario == WaterSpawnScenario::WATERFALL_CLIFF);
    merged.waterfallCliffHeight = m_PhysicsParams.waterfallCliffHeight;
    merged.waterfallCliffWidth = m_PhysicsParams.waterfallCliffWidth;
    merged.waterfallCliffDepth = m_PhysicsParams.waterfallCliffDepth;
    merged.waterfallFlowVolume = m_PhysicsParams.waterfallFlowVolume;
    merged.cliffLipSlopeDeg = m_PhysicsParams.cliffLipSlopeDeg;
    merged.cliffUndercut = m_PhysicsParams.cliffUndercut;
    merged.cliffSteppedLedges = m_PhysicsParams.cliffSteppedLedges;
    merged.cliffRoughness = m_PhysicsParams.cliffRoughness;
    merged.cliffRockChanneling = m_PhysicsParams.cliffRockChanneling;
    merged.airDragAndMist = m_PhysicsParams.airDragAndMist;
    merged.plungePoolDepth = m_PhysicsParams.plungePoolDepth;

    m_PhysicsParams = merged;
    m_RenderParams = render;
    m_Simulator.UpdateParams(m_PhysicsParams);
    m_Camera.SetTarget(Vec3(0.0f, std::clamp(merged.boundsHeight * 0.38f, 0.5f, 1.5f), 0.0f));
    m_Camera.SetOrbit(camYawDeg, camPitchDeg, camDistance);
}

void WaterSimulationEngine::ConfigureWaterfall(
    bool enabled,
    float cliffHeight,
    float cliffWidth,
    float cliffDepth,
    float flowVolume,
    float lipSlopeDeg,
    float undercut,
    int32_t steppedLedges,
    float roughness,
    float rockChanneling,
    float airDragAndMist,
    float plungePoolDepth
) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_PhysicsParams.waterfallEnabled = enabled;
    m_PhysicsParams.waterfallCliffHeight = std::clamp(cliffHeight, 0.55f, 2.30f);
    m_PhysicsParams.waterfallCliffWidth = std::clamp(cliffWidth, 0.40f, 2.60f);
    m_PhysicsParams.waterfallCliffDepth = std::clamp(cliffDepth, 0.35f, 1.80f);
    m_PhysicsParams.waterfallFlowVolume = std::clamp(flowVolume, 0.15f, 2.80f);
    m_PhysicsParams.cliffLipSlopeDeg = std::clamp(lipSlopeDeg, 0.0f, 45.0f);
    m_PhysicsParams.cliffUndercut = std::clamp(undercut, 0.0f, 0.50f);
    m_PhysicsParams.cliffSteppedLedges = std::clamp(steppedLedges, 0, 3);
    m_PhysicsParams.cliffRoughness = std::clamp(roughness, 0.0f, 1.0f);
    m_PhysicsParams.cliffRockChanneling = std::clamp(rockChanneling, 0.0f, 1.0f);
    m_PhysicsParams.airDragAndMist = std::clamp(airDragAndMist, 0.0f, 1.0f);
    m_PhysicsParams.plungePoolDepth = std::clamp(plungePoolDepth, 0.05f, 0.90f);
    m_Simulator.UpdateParams(m_PhysicsParams);
}

void WaterSimulationEngine::ResetSimulation(WaterSpawnScenario scenario, uint32_t particleCount) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_PhysicsParams.scenario = scenario;
    if (scenario == WaterSpawnScenario::WATERFALL_CLIFF) {
        m_PhysicsParams.waterfallEnabled = true;
    }
    m_PhysicsParams.particleCount = std::clamp(particleCount, 128u, 4096u);
    m_Simulator.Reset(m_PhysicsParams);
}

void WaterSimulationEngine::InjectScreenImpulse(
    float normU,
    float normV,
    float dragDx,
    float dragDy,
    float strength
) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    Vec3 rayOrigin, rayDir;
    m_Camera.ScreenToWorldRay(std::clamp(normU, 0.0f, 1.0f), std::clamp(normV, 0.0f, 1.0f), rayOrigin, rayDir);

    // Intersect ray with horizontal plane at mid-fluid height (y ~ 0.65m)
    float planeY = std::clamp(m_PhysicsParams.boundsHeight * 0.30f, 0.25f, 1.2f);
    float t = (std::abs(rayDir.y) > 1e-4f) ? ((planeY - rayOrigin.y) / rayDir.y) : 3.5f;
    t = std::clamp(t, 0.5f, 12.0f);
    Vec3 hitPoint = rayOrigin + rayDir * t;

    Vec3 impulse = (m_Camera.GetRight() * dragDx - m_Camera.GetUp() * dragDy) * strength;
    impulse.y += std::abs(dragDx) * 0.15f * strength;
    m_Simulator.InjectImpulse(hitPoint, impulse, 0.85f);
}

void WaterSimulationEngine::TriggerSplashPulse(int32_t pulseType, float strength) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    if (pulseType == 0) {
        // Center plunge / upward geyser
        m_Simulator.InjectImpulse(Vec3(0.0f, 0.35f, 0.0f), Vec3(0.0f, 4.8f * strength, 0.0f), 0.95f);
    } else if (pulseType == 1) {
        // Vortex swirl
        m_Simulator.InjectVortex(3.6f * strength);
    } else {
        // Lateral wall slosh wave
        m_Simulator.InjectImpulse(
            Vec3(-m_PhysicsParams.boundsWidth * 0.35f, 0.45f, 0.0f),
            Vec3(5.2f * strength, 1.4f * strength, 0.0f),
            1.35f
        );
    }
}

WaterTelemetry WaterSimulationEngine::StepAndRenderBitmap(
    uint32_t* pixelsRGBA8888,
    int32_t stridePixels,
    uint32_t width,
    uint32_t height,
    float dtSeconds,
    bool isPlaying
) {
    std::lock_guard<std::mutex> lock(m_Mutex);

    m_Camera.SetViewport(width, height, 48.0f);
    if (isPlaying) {
        m_Simulator.Step(dtSeconds);
    }

    m_Renderer.Render(
        pixelsRGBA8888,
        stridePixels,
        width,
        height,
        m_Simulator,
        m_Camera,
        m_RenderParams
    );

    WaterTelemetry telemetry{};
    telemetry.activeParticles = static_cast<uint32_t>(m_Simulator.GetPositions().size());
    telemetry.occupiedCells = m_Simulator.GetOccupiedCellCount();
    telemetry.hashTableSize = m_Simulator.GetHashTableSize();
    telemetry.physicsStepMs = m_Simulator.GetLastStepMs();
    telemetry.renderPassMs = m_Renderer.GetLastRenderMs();
    telemetry.avgDensity = m_Simulator.GetAverageDensity();
    telemetry.maxSpeed = m_Simulator.GetMaxSpeed();
    telemetry.kineticEnergy = m_Simulator.GetKineticEnergy();
    telemetry.vulkanGpuActive = sphere::vk::VulkanNativeSceneRenderer::IsGpuNodeAccessible();
    return telemetry;
}

WaterTelemetry WaterSimulationEngine::GetTelemetry() const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    WaterTelemetry telemetry{};
    telemetry.activeParticles = static_cast<uint32_t>(m_Simulator.GetPositions().size());
    telemetry.occupiedCells = m_Simulator.GetOccupiedCellCount();
    telemetry.hashTableSize = m_Simulator.GetHashTableSize();
    telemetry.physicsStepMs = m_Simulator.GetLastStepMs();
    telemetry.renderPassMs = m_Renderer.GetLastRenderMs();
    telemetry.avgDensity = m_Simulator.GetAverageDensity();
    telemetry.maxSpeed = m_Simulator.GetMaxSpeed();
    telemetry.kineticEnergy = m_Simulator.GetKineticEnergy();
    telemetry.vulkanGpuActive = sphere::vk::VulkanNativeSceneRenderer::IsGpuNodeAccessible();
    return telemetry;
}

} // namespace sphere::water
