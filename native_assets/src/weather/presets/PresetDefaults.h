#pragma once
#include "WeatherState.h"

namespace sphere::weather {

struct PresetDefaults {
    static constexpr WeatherPresetConfig CLEAR{"Clear", WeatherCondition::Clear, 0.0f, 0.1f, 0.01f, 5.0f, 45.0f, 22.0f, 0.4f};
    static constexpr WeatherPresetConfig RAIN{"Rain", WeatherCondition::Rain, 0.5f, 0.9f, 0.15f, 18.0f, 60.0f, 14.0f, 0.85f};
    static constexpr WeatherPresetConfig SNOW{"Snow", WeatherCondition::Snow, 0.4f, 0.8f, 0.20f, 12.0f, 90.0f, -2.0f, 0.70f};
    static constexpr WeatherPresetConfig STORM{"Storm", WeatherCondition::Thunderstorm, 0.95f, 0.98f, 0.35f, 48.0f, 135.0f, 15.0f, 0.95f};
};

} // namespace sphere::weather
