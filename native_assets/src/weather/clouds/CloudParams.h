#pragma once

namespace sphere::weather {

struct CloudParams {
    float coverage = 0.5f;          // [0..1]
    float density = 0.8f;           // optical thickness
    float baseAltitude = 1200.0f;   // meters
    float topAltitude = 3500.0f;    // meters
    float silverLining = 0.7f;      // forward scattering
    float windSpeed = 15.0f;        // km/h
};

} // namespace sphere::weather
