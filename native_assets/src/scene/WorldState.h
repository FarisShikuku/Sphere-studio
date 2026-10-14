#pragma once
#include "Transform.h"

namespace sphere {
struct WorldState {
    float time = 0.0f;
    uint32_t seed = 1337;
    float windSpeed = 5.0f;
    Vec3 windDirection{1.0f, 0.0f, 0.0f};
    float rainIntensity = 0.0f;
    float wetness = 0.0f;
    float snowDepth = 0.0f;
};
}\n