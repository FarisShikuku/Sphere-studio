#pragma once
#include <string>
#include "scene/Transform.h"

namespace sphere {

enum class WeatherCondition {
    Clear,
    Overcast,
    Rain,
    HeavyRain,
    Snow,
    Blizzard,
    Fog,
    Thunderstorm
};

struct WeatherState {
    WeatherCondition condition = WeatherCondition::Clear;
    float precipitationIntensity = 0.0f; // [0..1]
    float cloudCoverage = 0.15f;         // [0..1]
    float fogDensity = 0.02f;            // [0..1]
    float windSpeed = 8.5f;              // km/h
    float windAngleDegrees = 45.0f;      // azimuth
    Vec3 windDirection{0.707f, 0.0f, 0.707f};
    float lightningIntensity = 0.0f;     // [0..1]
    float wetness = 0.0f;                // [0..1]
    float puddleDepth = 0.0f;            // [0..1]
    float temperatureCelsius = 21.0f;
    float humidity = 0.45f;              // [0..1]
    float simulationTime = 0.0f;
    bool isStormy = false;
};

struct WeatherPresetConfig {
    const char* name;
    WeatherCondition condition;
    float precipitation;
    float cloudCoverage;
    float fogDensity;
    float windSpeed;
    float windAngle;
    float temperature;
    float humidity;
};

} // namespace sphere
