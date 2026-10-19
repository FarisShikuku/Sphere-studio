#include "WeatherPreset.h"

namespace sphere::weather {

WeatherState GetClearWeatherPreset() {
    WeatherState s;
    s.condition = WeatherCondition::Clear;
    s.precipitationIntensity = 0.0f;
    s.cloudCoverage = 0.05f;
    return s;
}

} // namespace sphere::weather
