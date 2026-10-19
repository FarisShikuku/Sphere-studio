#include <cassert>
#include <iostream>
#include "weather/sky/SkyGenerator.h"

int main() {
    sphere::weather::SkyGenerator sky;
    sphere::Vec3 rad = sky.ComputeSkyRadiance(sphere::Vec3{0, 1, 0}, sphere::Vec3{0, 0.5, 0.5}, 0.5f);
    assert(rad.y > 0.0f);
    std::cout << "[PASS] test_weather_sky radiance y: " << rad.y << std::endl;
    return 0;
}
