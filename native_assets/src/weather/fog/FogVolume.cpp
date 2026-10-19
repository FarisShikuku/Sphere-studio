#include "FogVolume.h"
#include <cmath>
#include <algorithm>

namespace sphere::weather {

float FogVolume::SampleDensity(const Vec3& worldPos) const {
    float deltaH = std::max(0.0f, worldPos.y - m_params.groundLevel);
    return m_params.density * std::exp(-deltaH * m_params.heightFalloff);
}

} // namespace sphere::weather
