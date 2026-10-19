#include "StormParams.h"

namespace sphere::weather {

void EnableHail(StormParams& params, float probability) {
    params.hailProbability = probability;
}

} // namespace sphere::weather
