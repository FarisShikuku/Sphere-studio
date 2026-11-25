#pragma once
// ============================================================================
// Desert3DScene.h - Native C++17 3D Desert & Cactus Scene
// Owns 10 Iconic Desert Species, LookAt Camera, Sand Pedestal & Atmospheric Shading.
// ============================================================================

#include "DesertSpeciesParams.h"
#include "DesertCactusGenerator.h"
#include "../common/VegetationMath.h"
#include <vector>
#include <cstdint>
#include <string>

namespace vegetation {
namespace desert {

struct DesertCameraOrbit {
    float azimuthDeg{34.0f};
    float elevationDeg{20.0f};
    float distanceMeters{3.6f};
    Vec3 targetPosition{0.0f, 0.65f, 0.0f};
};

struct DesertSceneStats {
    uint32_t vertexCount{0};
    uint32_t triangleCount{0};
    uint32_t ribCount{0};
    uint32_t areoleCount{0};
    uint32_t spineCount{0};
    float renderTimeMs{0.0f};
    std::string activeSpeciesName;
    std::string botanicalName;
    std::string biome;
};

/**
 * Desert3DScene - Native C++17 3D Desert Environment & Cactus Viewport Manager
 */
class Desert3DScene {
public:
    Desert3DScene();
    ~Desert3DScene() = default;

    void setCamera(float azimuthDeg, float elevationDeg, float distanceMeters);
    void setViewportSize(int width, int height);

    void setSpecies(DesertCactusArchetype archetype);
    void setParameters(const DesertSpeciesParams& params);
    const DesertSpeciesParams& getParameters() const { return m_params; }

    // Renders the 3D desert scene and cactus geometry into a 32-bit ARGB raster pixel buffer
    void renderSceneToArgbBuffer(int32_t* outArgbBuffer, DesertSceneStats* outStats = nullptr);

    const DesertCameraOrbit& getCamera() const { return m_camera; }

private:
    DesertCameraOrbit m_camera;
    DesertSpeciesParams m_params;
    int m_viewportWidth{720};
    int m_viewportHeight{1280};
    float m_simulationTimeSec{0.0f};
};

} // namespace desert
} // namespace vegetation
