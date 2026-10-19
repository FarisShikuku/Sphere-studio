#include "WetnessMap.h"
#include <algorithm>

namespace sphere::weather {

void WetnessMap::Update(float deltaTime, float rainIntensity) {
    if (rainIntensity > 0.05f) {
        m_currentWetness = std::min(1.0f, m_currentWetness + m_params.accumulationRate * rainIntensity * deltaTime);
    } else {
        m_currentWetness = std::max(0.0f, m_currentWetness - m_params.drainageRate * deltaTime);
    }
}

} // namespace sphere::weather
