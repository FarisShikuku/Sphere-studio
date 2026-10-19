#include <cassert>
#include <iostream>
#include "weather/wind/GlobalWind.h"

int main() {
    sphere::weather::GlobalWind wind;
    sphere::Vec3 sample = wind.SampleWind(sphere::Vec3{0.0f, 0.0f, 0.0f}, 0.5f);
    assert(sample.x != 0.0f || sample.z != 0.0f);
    std::cout << "[PASS] test_global_wind: " << sample.x << ", " << sample.z << std::endl;
    return 0;
}
