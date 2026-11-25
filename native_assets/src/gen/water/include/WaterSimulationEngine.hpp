#pragma once

#include "FluidParams.hpp"
#include "FluidSimulator.hpp"
#include "FluidRenderer.hpp"
#include "OrbitalCamera.hpp"
#include "VulkanFluidPipeline.hpp"
#include "WaterSurfaceParams.hpp"
#include "WaterSurfaceSimulator.hpp"
#include "WaterSurfaceRenderer.hpp"
#include "shared/IGenerator.h"
#include <mutex>
#include <string>

namespace sphere::water {

/**
 * WaterSimulationEngine
 *
 * Master controller for the gen/water SPH + SSFR + Tessendorf WaterSurface subsystem
 * inside Sphere Engine 3D. Integrates with GeneratorRegistry (GeneratorType::Water) and
 * provides thread-safe JNI entrypoints for real-time simulation, orbital camera control,
 * impulse injection, multi-pass SSFR, and 2D FFT Water Surface rendering.
 */
class WaterSimulationEngine : public sphere::IGenerator {
public:
    static WaterSimulationEngine& Get();

    WaterSimulationEngine();
    ~WaterSimulationEngine() override = default;

    // IGenerator interface
    const std::string& GetName() const override { return m_Name; }
    sphere::GeneratorType GetType() const override { return sphere::GeneratorType::Water; }
    bool Generate(sphere::GeneratorContext& context, std::shared_ptr<sphere::SceneNode> outNode) override;

    void Configure(
        const FluidPhysicsParams& physics,
        const FluidRenderParams& render,
        float camYawDeg,
        float camPitchDeg,
        float camDistance
    );

    void ConfigureWaterfall(
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
    );

    void ConfigureWaterSurface(const WaterSurfaceParams& params);

    void ResetSimulation(WaterSpawnScenario scenario, uint32_t particleCount);
    void InjectScreenImpulse(float normU, float normV, float dragDx, float dragDy, float strength);
    void TriggerSplashPulse(int32_t pulseType, float strength);

    WaterTelemetry StepAndRenderBitmap(
        uint32_t* pixelsRGBA8888,
        int32_t stridePixels,
        uint32_t width,
        uint32_t height,
        float dtSeconds,
        bool isPlaying
    );

    WaterTelemetry GetTelemetry() const;

private:
    std::string m_Name = "WaterSimulationEngine";
    mutable std::mutex m_Mutex;

    FluidSimulator m_Simulator;
    FluidRenderer m_Renderer;
    WaterSurfaceSimulator m_SurfaceSimulator;
    WaterSurfaceRenderer m_SurfaceRenderer;
    OrbitalCamera m_Camera;
    VulkanFluidPipeline m_VulkanPipeline;

    void SyncCameraForCurrentMode();

    FluidPhysicsParams m_PhysicsParams{};
    FluidRenderParams m_RenderParams{};
    WaterSurfaceParams m_SurfaceParams{};
    float m_BaseCamYawDeg = -34.0f;
    float m_BaseCamPitchDeg = 22.0f;
    float m_BaseCamDistance = 4.6f;
    bool m_Initialized = false;
};

} // namespace sphere::water
