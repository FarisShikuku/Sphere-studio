#pragma once
// ============================================================================
// HeliosVegetationScene.h - Native C++17 3D Scene & Radiation Viewport
// Integrates HeliosCanopyEngine radiation and HeliosPlantArchitecture 3D models.
// Renders to a 32-bit ARGB raster frame buffer directly.
// ============================================================================

#include "HeliosCanopyEngine.h"
#include "HeliosPlantArchitecture.h"
#include "../common/VegetationMath.h"
#include <cstdint>
#include <vector>

namespace vegetation {
namespace helios {

struct HeliosCameraOrbit {
    float azimuthDeg{32.0f};
    float elevationDeg{22.0f};
    float distanceMeters{4.2f};
    Vec3 targetPosition{0.0f, 0.65f, 0.0f};
};

struct HeliosSceneStats {
    uint32_t vertexCount{0};
    uint32_t triangleCount{0};
    float totalLAI{0.0f};
    float absorbedSolarFluxWM2{0.0f};
    float canopyPhotosynthesisRate{0.0f};
    float canopyTranspirationMmH{0.0f};
    float renderTimeMs{0.0f};
};

/**
 * HeliosVegetationScene - Native 3D Scene Manager for Helios Botanical Simulation
 */
class HeliosVegetationScene {
public:
    HeliosVegetationScene();
    ~HeliosVegetationScene() = default;

    void setCamera(float azimuthDeg, float elevationDeg, float distanceMeters);
    void setViewportSize(int width, int height);

    void setSolarRadiation(float directFlux, float diffuseFlux, float zenithDeg, float azimuthDeg);
    void setCanopyParameters(float totalLAI, float heightMeters, LeafAngleDistribution lad);

    void renderSceneToArgbBuffer(int32_t* outArgbBuffer, HeliosSceneStats* outStats = nullptr);

    const HeliosCameraOrbit& getCamera() const { return m_camera; }
    const HeliosCanopyState& getCanopyState() const { return m_canopyState; }

private:
    HeliosCameraOrbit m_camera;
    HeliosCanopyEngine m_canopyEngine;
    HeliosCanopyState m_canopyState;
    int m_viewportWidth{720};
    int m_viewportHeight{1280};
    float m_totalLAI{3.2f};
    float m_canopyHeightMeters{1.6f};
    LeafAngleDistribution m_lad{LeafAngleDistribution::SPHERICAL};
};

} // namespace helios
} // namespace vegetation
