#pragma once

#include <cstdint>

namespace sphere::audio {

struct WeatherAudioParams {
    bool enabled = false;
    bool muted = false;

    float rain_intensity = 0.0f;     // 0.0 .. 1.0 (drizzle -> torrential downpour)
    float snow_intensity = 0.0f;     // 0.0 .. 1.0 (hushed snow air)
    float wind_speed_kmh = 12.0f;    // 0.0 .. 90.0 km/h (breeze -> gale howl)
    float wind_azimuth_deg = 0.0f;   // 0.0 .. 360.0 deg
    float fog_density = 0.0f;        // 0.0 .. 1.0 (low atmospheric ambient bed)
    bool storm_active = false;       // intensifies gusts & enables thunder
    float lightning_frequency = 0.0f;
    float master_volume = 1.0f;
};

struct WeatherAudioTelemetry {
    int32_t active = 0;
    float rain_level = 0.0f;
    float wind_level = 0.0f;
    float thunder_level = 0.0f;
    int32_t has_active_sound = 0;
};

} // namespace sphere::audio
