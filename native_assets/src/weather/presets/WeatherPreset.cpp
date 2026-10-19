#include "WeatherPreset.h"
#include "PresetDefaults.h"

namespace sphere::weather {

WeatherState WeatherPreset::CreatePreset(const std::string& name) {
    WeatherState state;
    if (name == "Rain") {
        state.condition = WeatherCondition::Rain;
        state.precipitationIntensity = 0.5f;
        state.cloudCoverage = 0.9f;
    } else if (name == "Snow") {
        state.condition = WeatherCondition::Snow;
        state.precipitationIntensity = 0.45f;
        state.temperatureCelsius = -2.0f;
    } else if (name == "Storm") {
        state.condition = WeatherCondition::Thunderstorm;
        state.precipitationIntensity = 0.95f;
        state.isStormy = true;
    }
    return state;
}

} // namespace sphere::weather
