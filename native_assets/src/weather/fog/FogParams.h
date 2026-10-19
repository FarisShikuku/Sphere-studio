#pragma once

namespace sphere::weather {

struct FogParams {
    float density = 0.05f;          // global extinction
    float heightFalloff = 0.15f;    // exponential falloff
    float groundLevel = 0.0f;       // height baseline
    float maxDistance = 300.0f;     // meters
    float anisotropy = 0.65f;       // Henyey-Greenstein g parameter
};

} // namespace sphere::weather
