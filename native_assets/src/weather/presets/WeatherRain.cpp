#include "WeatherPreset.h"

namespace sphere::weather {

WeatherState GetRainWeatherPreset() {
    WeatherState s;
    s.condition = WeatherCondition::Rain;
    s.precipitationIntensity = 0.55f;
    s.cloudCoverage = 0.92f;
    return s;
}

} // namespace sphere::weather
