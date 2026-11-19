#pragma once

#include "FluidParams.hpp"
#include "FluidSimulator.hpp"
#include "FluidRenderer.hpp"
#include "OrbitalCamera.hpp"
#include "VulkanFluidPipeline.hpp"
#include "shared/IGenerator.h"
#include <mutex>
#include <string>

namespace sphere::water {

/**
 * WaterSimulationEngine
 *
 * Master controller for the gen/water SPH + SSFR subsystem inside Sphere Engine 3D.
 * Integrates with GeneratorRegistry (GeneratorType::Water) and provides thread-safe
 * JNI entrypoints for real-time simulation, orbital camera control, impulse injection,
 * and multi-pass Screen-Space Fluid Rendering.
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
    OrbitalCamera m_Camera;
    VulkanFluidPipeline m_VulkanPipeline;

    FluidPhysicsParams m_PhysicsParams{};
    FluidRenderParams m_RenderParams{};
    bool m_Initialized = false;
};

} // namespace sphere::water
