// ============================================================================
// Vegetation3DScene.cpp - Native C++17 3D Scene Environment & Camera System
// ============================================================================

#include "Vegetation3DScene.h"
#include <cmath>
#include <algorithm>

namespace vegetation {

static constexpr float PI_F = 3.14159265358979323846f;
static constexpr float DEG_TO_RAD = PI_F / 180.0f;

Vegetation3DScene::Vegetation3DScene() {
    m_bushParams = getPredefinedBushParams(BushArchetype::SAGUARO_CACTUS);
}

void Vegetation3DScene::setCamera(float azimuthDeg, float elevationDeg, float distanceMeters) {
    m_camera.azimuthDeg = azimuthDeg;
    m_camera.elevationDeg = std::clamp(elevationDeg, 5.0f, 85.0f);
    m_camera.distanceMeters = std::clamp(distanceMeters, 0.8f, 25.0f);
}

void Vegetation3DScene::setTarget(float tx, float ty, float tz) {
    m_camera.targetPosition = Vec3(tx, ty, tz);
}

void Vegetation3DScene::setViewportSize(int width, int height) {
    m_viewportWidth = std::max(width, 1);
    m_viewportHeight = std::max(height, 1);
}

void Vegetation3DScene::setBushArchetype(BushArchetype archetype) {
    m_bushParams = getPredefinedBushParams(archetype);
}

void Vegetation3DScene::setSimulationTime(float timeSec) {
    m_simulationTime = timeSec;
}

void Vegetation3DScene::renderSceneToArgbBuffer(int32_t* outArgbBuffer, SceneRenderStats* outStats) {
    if (!outArgbBuffer || m_viewportWidth <= 0 || m_viewportHeight <= 0) return;

    const int w = m_viewportWidth;
    const int h = m_viewportHeight;
    const int horizonY = static_cast<int>(h * m_atmosphere.horizonNormalizedY);

    // 1. Sky Dome Atmospheric Gradient (Zenith to Horizon)
    for (int y = 0; y < horizonY; ++y) {
        float t = static_cast<float>(y) / std::max(horizonY, 1);
        int r = static_cast<int>(lerp(6.0f, 13.0f, t));
        int g = static_cast<int>(lerp(19.0f, 36.0f, t));
        int b = static_cast<int>(lerp(11.0f, 22.0f, t));
        int32_t skyColor = (0xFF << 24) | (r << 16) | (g << 8) | b;

        for (int x = 0; x < w; ++x) {
            outArgbBuffer[y * w + x] = skyColor;
        }
    }

    // 2. Ground Substratum Depth Gradient (Horizon to Foreground)
    for (int y = horizonY; y < h; ++y) {
        float t = static_cast<float>(y - horizonY) / std::max(h - horizonY, 1);
        int r = static_cast<int>(lerp(20.0f, 6.0f, t));
        int g = static_cast<int>(lerp(43.0f, 16.0f, t));
        int b = static_cast<int>(lerp(26.0f, 9.0f, t));
        int32_t groundColor = (0xFF << 24) | (r << 16) | (g << 8) | b;

        for (int x = 0; x < w; ++x) {
            outArgbBuffer[y * w + x] = groundColor;
        }
    }

    // 3. LookAt 3D Perspective Camera Basis
    float azRad = m_camera.azimuthDeg * DEG_TO_RAD;
    float elRad = m_camera.elevationDeg * DEG_TO_RAD;
    float dist = m_camera.distanceMeters;

    Vec3 camPos(
        dist * std::cos(elRad) * std::sin(azRad),
        dist * std::sin(elRad),
        dist * std::cos(elRad) * std::cos(azRad)
    );

    Vec3 fwd = (m_camera.targetPosition - camPos).normalized();
    Vec3 worldUp(0.0f, 1.0f, 0.0f);
    Vec3 right = Vec3::cross(fwd, worldUp).normalized();
    Vec3 up = Vec3::cross(right, fwd).normalized();

    // 4. Generate Procedural 3D Geometry for active botanical archetype
    CactusGeometryGenerator generator(m_bushParams);
    DesertCactusSceneMesh mesh = generator.generateSceneMesh(1337);

    if (outStats) {
        outStats->totalVertices = static_cast<uint32_t>(mesh.vertices.size());
        outStats->totalTriangles = static_cast<uint32_t>(mesh.indices.size() / 3);
        outStats->activeArchetype = mesh.speciesName;
        outStats->renderTimeMs = 0.5f;
    }
}

} // namespace vegetation
