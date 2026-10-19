#pragma once

namespace sphere::weather {

struct StormParams {
    float stormIntensity = 0.85f;    // [0..1]
    float lightningRate = 0.25f;     // strikes per min
    float thunderDelay = 1.2f;       // seconds
    float hailProbability = 0.15f;
    float squallWindSpeed = 60.0f;   // km/h
};

} // namespace sphere::weather
