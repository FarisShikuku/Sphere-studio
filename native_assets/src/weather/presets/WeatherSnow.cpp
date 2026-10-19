#include "WeatherPreset.h"

namespace sphere::weather {

WeatherState GetSnowWeatherPreset() {
    WeatherState s;
    s.condition = WeatherCondition::Snow;
    s.precipitationIntensity = 0.5f;
    s.temperatureCelsius = -3.0f;
    return s;
}

} // namespace sphere::weather
