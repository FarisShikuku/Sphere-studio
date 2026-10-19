#pragma once
#include <string>
#include "WeatherState.h"

namespace sphere::weather {

class WeatherPreset {
public:
    static WeatherState CreatePreset(const std::string& name);
};

} // namespace sphere::weather
