#include <cassert>
#include <iostream>
#include "weather/fog/FogVolume.h"

int main() {
    sphere::weather::FogVolume fog;
    sphere::weather::FogParams params;
    params.density = 0.1f;
    params.groundLevel = 0.0f;
    params.heightFalloff = 0.2f;
    fog.SetParams(params);

    float dGround = fog.SampleDensity(sphere::Vec3{0.0f, 0.0f, 0.0f});
    float dHigh = fog.SampleDensity(sphere::Vec3{0.0f, 100.0f, 0.0f});
    assert(dGround > dHigh);
    std::cout << "[PASS] test_fog density gradient validated" << std::endl;
    return 0;
}
