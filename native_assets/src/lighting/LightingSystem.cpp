#include "LightingSystem.h"
#include <cmath>

namespace sphere::lighting {

LightingSystem::LightingSystem() {
    SetActiveScenario(LightingScenario::FLASHLIGHT_INSPECTION);
}

void LightingSystem::SetActiveScenario(LightingScenario scenario) {
    m_activeScenario = scenario;
    auto preset = GetScenarioPreset(scenario);
    m_primaryLight = preset.primaryLight;
    m_ambientIntensity = preset.ambientIntensity;
    m_ambientColor = preset.ambientColor;
    m_secondaryLights.clear();

    // If 3-point studio, add Fill light and Rim light
    if (scenario == LightingScenario::THREE_POINT_STUDIO) {
        // Soft Fill light
        Light fill;
        fill.type = LightType::Point;
        fill.position = Vec3{2.5f, 1.8f, 1.5f};
        fill.direction = Vec3{-0.6f, -0.4f, -0.4f};
        fill.color = Light::KelvinToRGB(4200.0f);
        fill.intensity = 1800.0f;
        fill.range = 15.0f;
        fill.castShadows = false;
        m_secondaryLights.push_back(fill);

        // High-rim backlight
        Light rim;
        rim.type = LightType::Spot;
        rim.position = Vec3{0.0f, 3.2f, -2.5f};
        rim.direction = Vec3{0.0f, -0.7f, 0.7f};
        rim.color = Light::KelvinToRGB(6500.0f);
        rim.intensity = 3200.0f;
        rim.range = 20.0f;
        rim.spotInnerAngle = 20.0f;
        rim.spotOuterAngle = 40.0f;
        rim.castShadows = false;
        m_secondaryLights.push_back(rim);
    }
}

static GpuLight ConvertToGpuLight(const Light& light, const Mat4& lightSpaceMat) {
    GpuLight gpu{};

    gpu.positionAndType = Vec4{
        light.position.x,
        light.position.y,
        light.position.z,
        static_cast<float>(light.type)
    };

    gpu.directionAndRange = Vec4{
        light.direction.x,
        light.direction.y,
        light.direction.z,
        light.range
    };

    Vec3 effColor = light.color;
    if (light.useColorTemperature) {
        effColor = Light::KelvinToRGB(light.colorKelvin);
    }
    gpu.colorAndIntensity = Vec4{
        effColor.x,
        effColor.y,
        effColor.z,
        light.intensity
    };

    float cosInner = std::cos(light.spotInnerAngle * 3.14159265f / 180.0f);
    float cosOuter = std::cos(light.spotOuterAngle * 3.14159265f / 180.0f);
    gpu.spotParams = Vec4{
        cosInner,
        cosOuter,
        light.spotFalloff,
        light.radius
    };

    gpu.areaParams = Vec4{
        light.areaWidth,
        light.areaHeight,
        light.volumetricScattering,
        light.castShadows ? 1.0f : 0.0f
    };

    gpu.lightSpaceMatrix = lightSpaceMat;
    return gpu;
}

GpuLight LightingSystem::BuildGpuLight() const {
    return ConvertToGpuLight(m_primaryLight, ComputeLightSpaceMatrix());
}

std::vector<GpuLight> LightingSystem::BuildAllGpuLights() const {
    std::vector<GpuLight> all;
    all.reserve(1 + m_secondaryLights.size());
    all.push_back(BuildGpuLight());

    for (const auto& sec : m_secondaryLights) {
        all.push_back(ConvertToGpuLight(sec, Mat4::Identity()));
    }
    return all;
}

Mat4 LightingSystem::ComputeLightSpaceMatrix() const {
    Vec3 lightPos = m_primaryLight.position;
    Vec3 lightDir = m_primaryLight.direction;
    Vec3 target = lightPos + lightDir;
    Vec3 up{0.0f, 1.0f, 0.0f};
    if (std::abs(lightDir.y) > 0.99f) {
        up = Vec3{0.0f, 0.0f, 1.0f};
    }

    Mat4 view = Mat4::LookAt(lightPos, target, up);
    Mat4 proj;

    if (m_primaryLight.type == LightType::Spot) {
        float fovDegrees = m_primaryLight.spotOuterAngle * 2.0f;
        proj = Mat4::Perspective(fovDegrees * 3.14159265f / 180.0f, 1.0f, 0.2f, m_primaryLight.range);
    } else {
        proj = Mat4::Perspective(90.0f * 3.14159265f / 180.0f, 1.0f, 0.2f, m_primaryLight.range);
    }

    return proj * view;
}

} // namespace sphere::lighting
