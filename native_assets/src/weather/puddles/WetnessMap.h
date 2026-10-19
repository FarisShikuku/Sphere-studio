#pragma once
#include <vector>
#include "PuddleParams.h"

namespace sphere::weather {

class WetnessMap {
public:
    WetnessMap() = default;
    void SetParams(const PuddleParams& params) { m_params = params; }
    const PuddleParams& GetParams() const { return m_params; }

    void Update(float deltaTime, float rainIntensity);
    float GetOverallWetness() const { return m_currentWetness; }

private:
    PuddleParams m_params{};
    float m_currentWetness = 0.0f;
};

} // namespace sphere::weather
