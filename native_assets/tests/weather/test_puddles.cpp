#include <cassert>
#include <iostream>
#include "weather/puddles/WetnessMap.h"

int main() {
    sphere::weather::WetnessMap wetness;
    wetness.Update(1.0f, 0.8f);
    assert(wetness.GetOverallWetness() > 0.0f);
    std::cout << "[PASS] test_puddles wetness: " << wetness.GetOverallWetness() << std::endl;
    return 0;
}
