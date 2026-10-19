#pragma once
#include "FogParams.h"
#include "scene/Transform.h"

namespace sphere::weather {

class FogVolume {
public:
    FogVolume() = default;
    void SetParams(const FogParams& params) { m_params = params; }
    const FogParams& GetParams() const { return m_params; }

    float SampleDensity(const Vec3& worldPos) const;

private:
    FogParams m_params{};
};

} // namespace sphere::weather
