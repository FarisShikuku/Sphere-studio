#include "StormParams.h"

namespace sphere::weather {

void ConfigureSevereThunderstorm(StormParams& params) {
    params.stormIntensity = 1.0f;
    params.lightningRate = 0.8f;
    params.squallWindSpeed = 75.0f;
}

} // namespace sphere::weather
