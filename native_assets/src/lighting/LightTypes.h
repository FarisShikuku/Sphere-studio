#pragma once
#include "scene/Light.h"
#include <string>
#include <vector>

namespace sphere::lighting {

/**
 * Lighting Scenario presets referencing Gaukler/PlainRenderer.
 */
enum class LightingScenario {
    FLASHLIGHT_INSPECTION = 0,   // Handheld tactical flashlight cone with soft penumbra in dark environment
    WARM_FILAMENT_BULB = 1,      // 2700K warm tungsten bare bulb hanging over stage with inverse-square falloff
    STUDIO_SOFTBOX_AREA = 2,     // 1.2m x 0.8m rectangular softbox area light providing diffused highlights
    THREE_POINT_STUDIO = 3,      // Key light, soft Fill light, and high-intensity Rim light
    CYBERPUNK_NEON = 4,          // Dual high-chroma colored spots (Cyan/Magenta) with volumetric fog haze
    STREET_LAMP_POST = 5         // Overhead downward-directed sodium vapor street lamp on a pole
};

struct LightScenarioPreset {
    LightingScenario scenario;
    const char* id;
    const char* name;
    const char* description;
    Light primaryLight;
    float ambientIntensity;
    Vec3 ambientColor;
};

// Preset database
inline LightScenarioPreset GetScenarioPreset(LightingScenario scenario) {
    LightScenarioPreset p{};
    p.scenario = scenario;

    switch (scenario) {
        case LightingScenario::FLASHLIGHT_INSPECTION: {
            p.id = "flashlight_inspection";
            p.name = "Tactical Flashlight";
            p.description = "Narrow high-intensity spot beam with smooth penumbra cutoff and dark ambient shadow contrast.";
            p.primaryLight.type = LightType::Spot;
            p.primaryLight.position = Vec3{0.0f, 2.2f, 2.8f};
            p.primaryLight.direction = Vec3{0.0f, -0.45f, -0.89f};
            p.primaryLight.color = Vec3{0.92f, 0.96f, 1.0f};
            p.primaryLight.intensity = 4200.0f;
            p.primaryLight.colorKelvin = 5800.0f;
            p.primaryLight.range = 25.0f;
            p.primaryLight.spotInnerAngle = 14.0f;
            p.primaryLight.spotOuterAngle = 28.0f;
            p.primaryLight.spotFalloff = 1.0f;
            p.primaryLight.enableVolumetrics = true;
            p.primaryLight.volumetricScattering = 0.85f;
            p.ambientIntensity = 0.05f;
            p.ambientColor = Vec3{0.08f, 0.10f, 0.14f};
            break;
        }
        case LightingScenario::WARM_FILAMENT_BULB: {
            p.id = "warm_filament_bulb";
            p.name = "Hanging Filament Bulb";
            p.description = "Omnidirectional 2700K warm tungsten point light casting radial shadows with inverse-square falloff.";
            p.primaryLight.type = LightType::Point;
            p.primaryLight.position = Vec3{0.0f, 2.4f, 0.0f};
            p.primaryLight.direction = Vec3{0.0f, -1.0f, 0.0f};
            p.primaryLight.color = Light::KelvinToRGB(2700.0f);
            p.primaryLight.intensity = 1800.0f;
            p.primaryLight.colorKelvin = 2700.0f;
            p.primaryLight.range = 14.0f;
            p.primaryLight.radius = 0.08f;
            p.primaryLight.enableVolumetrics = true;
            p.primaryLight.volumetricScattering = 0.40f;
            p.ambientIntensity = 0.12f;
            p.ambientColor = Vec3{0.18f, 0.14f, 0.10f};
            break;
        }
        case LightingScenario::STUDIO_SOFTBOX_AREA: {
            p.id = "studio_softbox_area";
            p.name = "Architectural Softbox";
            p.description = "1.4m x 0.9m rectangular diffuse emitter panel producing broad, gentle specular reflections.";
            p.primaryLight.type = LightType::Area;
            p.primaryLight.position = Vec3{1.8f, 3.0f, 2.0f};
            p.primaryLight.direction = Vec3{-0.45f, -0.70f, -0.55f};
            p.primaryLight.color = Light::KelvinToRGB(4800.0f);
            p.primaryLight.intensity = 5500.0f;
            p.primaryLight.colorKelvin = 4800.0f;
            p.primaryLight.range = 20.0f;
            p.primaryLight.areaWidth = 1.4f;
            p.primaryLight.areaHeight = 0.9f;
            p.primaryLight.areaNormal = Vec3{-0.45f, -0.70f, -0.55f};
            p.primaryLight.enableVolumetrics = false;
            p.primaryLight.volumetricScattering = 0.20f;
            p.ambientIntensity = 0.20f;
            p.ambientColor = Vec3{0.20f, 0.22f, 0.26f};
            break;
        }
        case LightingScenario::THREE_POINT_STUDIO: {
            p.id = "three_point_studio";
            p.name = "3-Point Studio Lighting";
            p.description = "Classic photographic studio setup with high-contrast Key light, soft Fill, and rim silhouette.";
            p.primaryLight.type = LightType::Spot;
            p.primaryLight.position = Vec3{-2.2f, 3.4f, 2.5f};
            p.primaryLight.direction = Vec3{0.55f, -0.70f, -0.45f};
            p.primaryLight.color = Light::KelvinToRGB(5200.0f);
            p.primaryLight.intensity = 4800.0f;
            p.primaryLight.colorKelvin = 5200.0f;
            p.primaryLight.range = 22.0f;
            p.primaryLight.spotInnerAngle = 24.0f;
            p.primaryLight.spotOuterAngle = 45.0f;
            p.primaryLight.enableVolumetrics = true;
            p.primaryLight.volumetricScattering = 0.35f;
            p.ambientIntensity = 0.25f;
            p.ambientColor = Vec3{0.22f, 0.24f, 0.30f};
            break;
        }
        case LightingScenario::CYBERPUNK_NEON: {
            p.id = "cyberpunk_neon";
            p.name = "Cyberpunk Neon Saturated";
            p.description = "Vibrant electric cyan spot beam with high volumetric fog scattering and purple ambient.";
            p.primaryLight.type = LightType::Spot;
            p.primaryLight.position = Vec3{1.5f, 2.8f, 1.8f};
            p.primaryLight.direction = Vec3{-0.45f, -0.75f, -0.48f};
            p.primaryLight.color = Vec3{0.10f, 0.85f, 1.0f}; // Electric Cyan
            p.primaryLight.intensity = 6000.0f;
            p.primaryLight.useColorTemperature = false;
            p.primaryLight.range = 24.0f;
            p.primaryLight.spotInnerAngle = 20.0f;
            p.primaryLight.spotOuterAngle = 42.0f;
            p.primaryLight.enableVolumetrics = true;
            p.primaryLight.volumetricScattering = 0.95f; // Heavy fog beam
            p.ambientIntensity = 0.15f;
            p.ambientColor = Vec3{0.30f, 0.08f, 0.35f}; // Purple neon haze
            break;
        }
        case LightingScenario::STREET_LAMP_POST: {
            p.id = "street_lamp_post";
            p.name = "Sodium Street Lamp";
            p.description = "Overhead 2200K high-pressure sodium street luminaire mounted on a post illuminating the ground.";
            p.primaryLight.type = LightType::Spot;
            p.primaryLight.position = Vec3{0.0f, 3.8f, 0.0f};
            p.primaryLight.direction = Vec3{0.0f, -1.0f, 0.0f}; // Directly down
            p.primaryLight.color = Light::KelvinToRGB(2200.0f); // Warm Amber
            p.primaryLight.intensity = 3800.0f;
            p.primaryLight.colorKelvin = 2200.0f;
            p.primaryLight.range = 16.0f;
            p.primaryLight.spotInnerAngle = 32.0f;
            p.primaryLight.spotOuterAngle = 58.0f;
            p.primaryLight.enableVolumetrics = true;
            p.primaryLight.volumetricScattering = 0.70f;
            p.ambientIntensity = 0.08f;
            p.ambientColor = Vec3{0.12f, 0.10f, 0.08f};
            break;
        }
    }
    return p;
}

} // namespace sphere::lighting
