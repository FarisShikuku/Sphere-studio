#pragma once
#include "Transform.h"
#include <cmath>
#include <algorithm>

namespace sphere {

/**
 * Light types supported by the Vulkan rendering pipeline,
 * referencing Gaukler/PlainRenderer physically based light architecture.
 */
enum class LightType {
    Directional = 0,
    Point = 1,       // Omnidirectional Filament / LED Bulb
    Spot = 2,        // Flashlight / Stage Follow-Spot
    Area = 3         // Rectangular Architectural / Studio Softbox Light
};

enum class LightUnit {
    Lux = 0,         // Illuminance (lm/m^2) for Directional
    Lumens = 1,      // Luminous flux for Point & Area
    Candela = 2      // Luminous intensity for Spot
};

enum class ShadowFilter {
    Hard = 0,
    PCF_3x3 = 1,
    PCF_5x5 = 2,
    CSM_4 = 3
};

/**
 * Physically based light source definition referencing Gaukler/PlainRenderer.
 */
struct Light {
    LightType type = LightType::Spot;
    LightUnit unit = LightUnit::Lumens;
    
    // Position & Orientation
    Vec3 position{0.0f, 3.5f, 2.5f};
    Vec3 direction{0.0f, -0.8f, -0.6f}; // Normalized pointing vector
    
    // Photometric Color & Intensity
    Vec3 color{1.0f, 0.95f, 0.88f};     // Linear sRGB color tint
    float intensity = 2500.0f;          // Lumens / Lux / Candela
    float colorKelvin = 3200.0f;        // Blackbody temperature (e.g. 2700K warm tungsten, 5500K daylight)
    bool useColorTemperature = true;
    
    // Range & Attenuation (Gaukler/PlainRenderer inverse-square physical falloff)
    float range = 18.0f;                // Maximum effective reach in meters
    float radius = 0.12f;               // Emissive source radius in meters (bulb size)
    
    // Spot Light / Flashlight Parameters
    float spotInnerAngle = 18.0f;       // Degrees: Inner full-intensity cone
    float spotOuterAngle = 36.0f;       // Degrees: Outer falloff penumbra cone
    float spotFalloff = 1.0f;           // Smoothstep cosine falloff exponent
    
    // Area Light Parameters (Rectangular Softbox / Emitter Panel)
    float areaWidth = 1.2f;             // Width in meters
    float areaHeight = 0.8f;            // Height in meters
    Vec3 areaNormal{0.0f, -0.7f, -0.7f};
    bool areaTwoSided = false;
    
    // Volumetrics & Media (Froxel fog shaft scattering)
    bool enableVolumetrics = true;
    float volumetricScattering = 0.65f; // In-scattering coefficient for visible light beam
    
    // Shadow Mapping Parameters (Sascha Willems / Gaukler PCF)
    bool castShadows = true;
    ShadowFilter shadowFilter = ShadowFilter::PCF_5x5;
    float shadowBias = 0.005f;
    float shadowSlopeBias = 0.015f;
    float pcfRadius = 1.5f;
    uint32_t shadowMapResolution = 2048;

    // Helper: Convert Kelvin temperature to linear RGB blackbody radiation
    static Vec3 KelvinToRGB(float k) {
        float temp = std::clamp(k, 1000.0f, 15000.0f) / 100.0f;
        float r, g, b;

        // Red
        if (temp <= 66.0f) {
            r = 1.0f;
        } else {
            r = std::clamp(329.698727446f * std::pow(temp - 60.0f, -0.1332047592f) / 255.0f, 0.0f, 1.0f);
        }

        // Green
        if (temp <= 66.0f) {
            g = std::clamp((99.4708025861f * std::log(temp) - 161.1195681661f) / 255.0f, 0.0f, 1.0f);
        } else {
            g = std::clamp(288.1221695283f * std::pow(temp - 60.0f, -0.0755148492f) / 255.0f, 0.0f, 1.0f);
        }

        // Blue
        if (temp >= 66.0f) {
            b = 1.0f;
        } else if (temp <= 19.0f) {
            b = 0.0f;
        } else {
            b = std::clamp((138.5177312231f * std::log(temp - 10.0f) - 305.0447927307f) / 255.0f, 0.0f, 1.0f);
        }

        return Vec3{r, g, b};
    }
};

/**
 * GPU std430 uniform block representation for Vulkan UBO / SSBO binding.
 */
struct alignas(16) GpuLight {
    Vec4 positionAndType;       // xyz = worldPos, w = float(LightType)
    Vec4 directionAndRange;     // xyz = worldDir, w = range
    Vec4 colorAndIntensity;     // xyz = rgb, w = intensity
    Vec4 spotParams;            // x = cos(inner), y = cos(outer), z = spotFalloff, w = sourceRadius
    Vec4 areaParams;            // x = areaWidth, y = areaHeight, z = volumetricScattering, w = castShadows
    Mat4 lightSpaceMatrix;      // Light-space projection matrix for shadow mapping
};

} // namespace sphere