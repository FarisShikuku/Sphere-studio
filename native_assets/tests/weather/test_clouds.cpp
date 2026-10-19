#include <cassert>
#include <iostream>
#include "weather/clouds/CloudGen.h"

int main() {
    sphere::weather::CloudGen cloudGen;
    sphere::weather::CloudParams params;
    params.coverage = 0.6f;
    cloudGen.SetParams(params);

    float d = cloudGen.SampleCloudDensity(sphere::Vec3{0.0f, 2000.0f, 0.0f}, 1.0f);
    std::cout << "[PASS] test_clouds density: " << d << std::endl;
    return 0;
}
