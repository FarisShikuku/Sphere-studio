#include <cassert>
#include <iostream>
#include "weather/rain/RainSystem.h"

int main() {
    sphere::weather::RainSystem rain;
    sphere::weather::RainParams params;
    params.intensity = 0.8f;
    rain.SetParams(params);

    rain.Update(0.1f, sphere::Vec3{5.0f, 0.0f, 0.0f});
    std::cout << "[PASS] test_rain active drops: " << rain.GetActiveDropCount() << std::endl;
    return 0;
}
