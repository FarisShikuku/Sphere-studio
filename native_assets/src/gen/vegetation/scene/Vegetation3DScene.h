#pragma once
// ============================================================================
// Vegetation3DScene.h - Native C++17 3D Scene Environment & Camera System
// Owns the complete 3D scene architecture: LookAt camera, atmospheric sky dome,
// concentric ground pedestal, contact drop shadows, and vegetation mesh rendering.
// ============================================================================

#include "../common/VegetationMath.h"
#include "../bushes/BushParams.h"
#include "../bushes/CactusGeometryGenerator.h"
#include <vector>
#include <cstdint>
#include <string>

namespace vegetation {

struct SceneCameraOrbit {
    float azimuthDeg{36.0f};
    float elevationDeg{24.0f};
    float distanceMeters{3.8f};
    Vec3 targetPosition{0.0f, 0.45f, 0.0f};
};

struct SceneGroundPedestal {
    float baseRadiusMeters{1.8f};
    int ringCount{3};
    int spokeCount{8};
    uint32_t ringColorArgb{0x33285842};
    uint32_t shadowColorArgb{0x66020A05};
};

struct SceneAtmosphereDome {
    uint32_t skyZenithArgb{0xFF06130B};
    uint32_t skyHorizonArgb{0xFF0D2416};
    uint32_t groundNearArgb{0xFF142B1A};
    uint32_t groundFarArgb{0xFF061009};
    float horizonNormalizedY{0.44f};
};

struct SceneRenderStats {
    uint32_t totalVertices{0};
    uint32_t totalTriangles{0};
    float renderTimeMs{0.0f};
    std::string activeArchetype;
};

/**
 * Vegetation3DScene - Native C++17 Master 3D Scene Manager
 * Fully owns all scene calculations:
 * - View & Projection matrices (Orthonormal LookAt basis)
 * - Atmospheric Sky Dome gradient synthesis
 * - Ground Pedestal & Shadow projection
 * - Procedural Vegetation & Desert Cactus geometry generation
 * - Software frame rasterizer producing 32-bit ARGB native pixel buffers
 */
class Vegetation3DScene {
public:
    Vegetation3DScene();
    ~Vegetation3DScene() = default;

    void setCamera(float azimuthDeg, float elevationDeg, float distanceMeters);
    void setTarget(float tx, float ty, float tz);
    void setViewportSize(int width, int height);

    void setBushArchetype(BushArchetype archetype);
    void setSimulationTime(float timeSec);

    // Render full 3D scene directly into an ARGB_8888 native pixel buffer
    void renderSceneToArgbBuffer(int32_t* outArgbBuffer, SceneRenderStats* outStats = nullptr);

    const SceneCameraOrbit& getCamera() const { return m_camera; }
    const BushSynthesisParams& getParams() const { return m_bushParams; }

private:
    SceneCameraOrbit m_camera;
    SceneGroundPedestal m_pedestal;
    SceneAtmosphereDome m_atmosphere;
    BushSynthesisParams m_bushParams;
    int m_viewportWidth{720};
    int m_viewportHeight{1280};
    float m_simulationTime{0.0f};
};

} // namespace vegetation
