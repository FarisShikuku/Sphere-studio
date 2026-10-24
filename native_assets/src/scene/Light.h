#pragma once
#include "Transform.h"

namespace sphere {
enum class LightType { Directional, Point, Spot };

enum class ShadowFilter {
    Hard = 0,
    PCF_3x3 = 1,
    PCF_5x5 = 2,
    CSM_4 = 3
};

struct Light {
    LightType type = LightType::Directional;
    Vec3 color{1.0f, 1.0f, 1.0f};
    float intensity = 1.0f;
    Vec3 position{0.0f, 10.0f, 0.0f};
    Vec3 direction{-0.5f, -1.0f, -0.3f};
    float range = 25.0f;
    float spotAngle = 45.0f;

    // Shadow Mapping Parameters (SaschaWillems/Vulkan reference)
    bool castShadows = true;
    ShadowFilter shadowFilter = ShadowFilter::PCF_5x5;
    float shadowBias = 0.005f;
    float shadowSlopeBias = 0.015f;
    float pcfRadius = 1.5f;
    uint32_t shadowMapResolution = 2048;
};
}