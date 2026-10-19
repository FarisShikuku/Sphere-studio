#pragma once

namespace sphere::weather {

struct SnowParams {
    float intensity = 0.5f;        // [0..1]
    float fallSpeed = 1.8f;        // m/s
    float flakeSize = 0.015f;      // meters
    float flutterFrequency = 4.0f; // Hz
    float flutterAmplitude = 0.25f;
    float accumulationRate = 0.005f;
};

} // namespace sphere::weather
