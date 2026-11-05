#pragma once
#include "LightTypes.h"
#include <vector>
#include <memory>

namespace sphere::lighting {

/**
 * LightingSystem - Master lighting coordinator referencing Gaukler/PlainRenderer.
 * Manages active punctual, spot, bulb, and area lights, builds std430 uniform blocks,
 * and computes shadow projection matrices.
 */
class LightingSystem {
public:
    LightingSystem();
    ~LightingSystem() = default;

    void SetActiveScenario(LightingScenario scenario);
    LightingScenario GetActiveScenario() const { return m_activeScenario; }

    Light& GetPrimaryLight() { return m_primaryLight; }
    const Light& GetPrimaryLight() const { return m_primaryLight; }
    void SetPrimaryLight(const Light& light) { m_primaryLight = light; }

    // Multi-light management
    void AddSecondaryLight(const Light& light) { m_secondaryLights.push_back(light); }
    void ClearSecondaryLights() { m_secondaryLights.clear(); }
    const std::vector<Light>& GetSecondaryLights() const { return m_secondaryLights; }
    size_t GetTotalLightCount() const { return 1 + m_secondaryLights.size(); }

    // Generates GpuLight data for Vulkan uniform buffer
    GpuLight BuildGpuLight() const;

    // Generates array of all GPU lights (std430 layout)
    std::vector<GpuLight> BuildAllGpuLights() const;

    // Generates shadow view-projection matrix for active light
    Mat4 ComputeLightSpaceMatrix() const;

    float GetAmbientIntensity() const { return m_ambientIntensity; }
    void SetAmbientIntensity(float val) { m_ambientIntensity = val; }

    Vec3 GetAmbientColor() const { return m_ambientColor; }
    void SetAmbientColor(const Vec3& col) { m_ambientColor = col; }

private:
    LightingScenario m_activeScenario = LightingScenario::FLASHLIGHT_INSPECTION;
    Light m_primaryLight;
    std::vector<Light> m_secondaryLights;
    float m_ambientIntensity = 0.08f;
    Vec3 m_ambientColor{0.10f, 0.12f, 0.16f};
};

} // namespace sphere::lighting
