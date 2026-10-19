#pragma once

namespace sphere::weather {

struct RainParams {
    float intensity = 0.5f;        // [0..1]
    float dropSpeed = 16.0f;       // m/s
    float dropLength = 0.35f;      // meters
    float dropletRadius = 0.002f;  // meters
    float splashProbability = 0.65f;
    float windDeflection = 1.0f;
};

} // namespace sphere::weather
