#include "WeatherPreset.h"

namespace sphere::weather {

WeatherState GetStormWeatherPreset() {
    WeatherState s;
    s.condition = WeatherCondition::Thunderstorm;
    s.precipitationIntensity = 0.95f;
    s.isStormy = true;
    return s;
}

} // namespace sphere::weather
