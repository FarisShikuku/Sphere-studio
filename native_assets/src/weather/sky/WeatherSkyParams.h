#pragma once

namespace sphere::weather {

struct WeatherSkyParams {
    float overcastTurbidity = 8.5f;
    float stormDarkening = 0.85f;
    float horizonHaze = 0.4f;
};

} // namespace sphere::weather
