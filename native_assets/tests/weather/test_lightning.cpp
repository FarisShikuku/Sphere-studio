#include <cassert>
#include <iostream>
#include "weather/lightning/LightningGen.h"

int main() {
    sphere::weather::LightningGen gen;
    auto bolt = gen.GenerateBolt(sphere::Vec3{0, 50, 0}, sphere::Vec3{0, 0, 0});
    assert(!bolt.empty());
    std::cout << "[PASS] test_lightning segments: " << bolt.size() << std::endl;
    return 0;
}
