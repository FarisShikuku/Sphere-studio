#pragma once
#include "Transform.h"

namespace sphere {
enum class LightType { Directional, Point, Spot };

struct Light {
    LightType type = LightType::Directional;
    Vec3 color{1.0f, 1.0f, 1.0f};
    float intensity = 1.0f;
    Vec3 position{0.0f, 10.0f, 0.0f};
    Vec3 direction{-0.5f, -1.0f, -0.3f};
    float range = 25.0f;
    float spotAngle = 45.0f;
};
}\n