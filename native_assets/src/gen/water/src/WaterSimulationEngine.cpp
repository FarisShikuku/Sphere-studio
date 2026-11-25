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
    m_SurfaceSimulator.Configure(m_SurfaceParams);
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
        m_SurfaceSimulator.Configure(m_SurfaceParams);
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
    m_BaseCamYawDeg = camYawDeg;
    m_BaseCamPitchDeg = camPitchDeg;
    m_BaseCamDistance = camDistance;
    m_Simulator.UpdateParams(m_PhysicsParams);
    SyncCameraForCurrentMode();
}

void WaterSimulationEngine::SyncCameraForCurrentMode() {
    if (m_SurfaceParams.enabled) {
        if (m_SurfaceParams.cutoutEnabled) {
            float maxSpan = std::max({
                m_SurfaceParams.cutoutWidthM,
                m_SurfaceParams.cutoutHeightM,
                m_SurfaceParams.cutoutDepthM
            });
            float scaledDist = std::max(3.6f, m_BaseCamDistance * std::max(1.0f, maxSpan / 3.4f));
            m_Camera.SetTarget(Vec3(0.0f, -m_SurfaceParams.cutoutHeightM * 0.42f, 0.0f));
            m_Camera.SetOrbit(m_BaseCamYawDeg, std::clamp(m_BaseCamPitchDeg, -25.0f, 82.0f), scaledDist);
        } else {
            m_Camera.SetTarget(Vec3(0.0f, 0.35f, 0.0f));
            m_Camera.SetOrbit(m_BaseCamYawDeg, std::clamp(m_BaseCamPitchDeg, 6.0f, 82.0f), std::max(m_BaseCamDistance, 3.2f));
        }
    } else {
        m_Camera.SetTarget(Vec3(0.0f, std::clamp(m_PhysicsParams.boundsHeight * 0.38f, 0.5f, 1.5f), 0.0f));
        m_Camera.SetOrbit(m_BaseCamYawDeg, m_BaseCamPitchDeg, m_BaseCamDistance);
    }
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

void WaterSimulationEngine::ConfigureWaterSurface(const WaterSurfaceParams& params) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_SurfaceParams = params;
    m_SurfaceSimulator.Configure(params);
    SyncCameraForCurrentMode();
}

void WaterSimulationEngine::ResetSimulation(WaterSpawnScenario scenario, uint32_t particleCount) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    if (m_SurfaceParams.enabled) {
        m_SurfaceSimulator.Reset(m_SurfaceParams);
        return;
    }
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

    if (m_SurfaceParams.enabled) {
        float t = (std::abs(rayDir.y) > 1e-4f) ? (-rayOrigin.y / rayDir.y) : 5.0f;
        t = std::clamp(t, 0.8f, 48.0f);
        Vec3 hitPoint = rayOrigin + rayDir * t;
        if (m_SurfaceParams.cutoutEnabled) {
            float hx = std::max(1.0f, m_SurfaceParams.cutoutWidthM * 0.5f);
            float hz = std::max(1.0f, m_SurfaceParams.cutoutDepthM * 0.5f);
            hitPoint.x = std::clamp(hitPoint.x, -hx, hx);
            hitPoint.z = std::clamp(hitPoint.z, -hz, hz);
        }
        m_SurfaceSimulator.InjectDisturbance(hitPoint.x, hitPoint.z, strength * 0.95f);
        return;
    }

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
    if (m_SurfaceParams.enabled) {
        m_SurfaceSimulator.TriggerPulse(pulseType, strength);
        return;
    }
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
    WaterTelemetry telemetry{};

    if (m_SurfaceParams.enabled) {
        if (isPlaying) {
            m_SurfaceSimulator.Step(dtSeconds);
        }
        m_SurfaceRenderer.Render(
            pixelsRGBA8888,
            stridePixels,
            width,
            height,
            m_SurfaceSimulator,
            m_Camera
        );
        uint32_t n = static_cast<uint32_t>(m_SurfaceSimulator.GetParams().gridResolution);
        telemetry.activeParticles = n * n;
        telemetry.occupiedCells = n;
        telemetry.hashTableSize = n * n * 2u;
        telemetry.physicsStepMs = m_SurfaceSimulator.GetLastFftMs();
        telemetry.renderPassMs = m_SurfaceRenderer.GetLastRenderMs();
        telemetry.avgDensity = m_SurfaceSimulator.GetSignificantWaveHeightM();
        telemetry.maxSpeed = m_SurfaceSimulator.GetPeakCrestM();
        telemetry.kineticEnergy = m_SurfaceSimulator.GetParams().windSpeedMs;
        telemetry.vulkanGpuActive = sphere::vk::VulkanNativeSceneRenderer::IsGpuNodeAccessible();
        return telemetry;
    }

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
    if (m_SurfaceParams.enabled) {
        uint32_t n = static_cast<uint32_t>(m_SurfaceSimulator.GetParams().gridResolution);
        telemetry.activeParticles = n * n;
        telemetry.occupiedCells = n;
        telemetry.hashTableSize = n * n * 2u;
        telemetry.physicsStepMs = m_SurfaceSimulator.GetLastFftMs();
        telemetry.renderPassMs = m_SurfaceRenderer.GetLastRenderMs();
        telemetry.avgDensity = m_SurfaceSimulator.GetSignificantWaveHeightM();
        telemetry.maxSpeed = m_SurfaceSimulator.GetPeakCrestM();
        telemetry.kineticEnergy = m_SurfaceSimulator.GetParams().windSpeedMs;
        telemetry.vulkanGpuActive = sphere::vk::VulkanNativeSceneRenderer::IsGpuNodeAccessible();
        return telemetry;
    }
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
