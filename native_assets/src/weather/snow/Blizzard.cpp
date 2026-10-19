#include "SnowSystem.h"

namespace sphere::weather {

void ConfigureBlizzard(SnowParams& params) {
    params.intensity = 1.0f;
    params.fallSpeed = 4.5f;
    params.flutterAmplitude = 1.2f;
    params.flutterFrequency = 12.0f;
    params.accumulationRate = 0.05f;
}

} // namespace sphere::weather
