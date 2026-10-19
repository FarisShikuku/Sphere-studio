#pragma once
#include "scene/Transform.h"

namespace sphere::weather {

struct WindParams {
    float baseSpeed = 10.0f;       // km/h
    float gustFrequency = 0.2f;    // Hz
    float gustMagnitude = 8.0f;    // km/h
    float turbulenceScale = 0.05f;
    float azimuthDegrees = 45.0f;
};

} // namespace sphere::weather
