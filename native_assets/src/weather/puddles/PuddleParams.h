#pragma once

namespace sphere::weather {

struct PuddleParams {
    float maxDepth = 0.04f;        // meters
    float drainageRate = 0.01f;    // m/s
    float accumulationRate = 0.05f;// m/s
    float rippleIntensity = 0.6f;
};

} // namespace sphere::weather
