#include <cassert>
#include <iostream>
#include "weather/presets/WeatherPreset.h"

int main() {
    auto rain = sphere::weather::WeatherPreset::CreatePreset("Rain");
    assert(rain.condition == sphere::WeatherCondition::Rain);
    auto storm = sphere::weather::WeatherPreset::CreatePreset("Storm");
    assert(storm.isStormy);
    std::cout << "[PASS] test_presets rain and storm verified" << std::endl;
    return 0;
}
