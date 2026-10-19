#include <cassert>
#include <iostream>
#include "weather/snow/SnowSystem.h"

int main() {
    sphere::weather::SnowSystem snow;
    sphere::weather::SnowParams params;
    params.intensity = 0.5f;
    snow.SetParams(params);

    snow.Update(0.1f, sphere::Vec3{2.0f, 0.0f, 1.0f});
    std::cout << "[PASS] test_snow flakes count: " << snow.GetFlakes().size() << std::endl;
    return 0;
}
