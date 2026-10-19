#pragma once
#include "WindParams.h"
#include "scene/Transform.h"

namespace sphere::weather {

class GlobalWind {
public:
    GlobalWind() = default;
    void SetParams(const WindParams& params) { m_params = params; }
    const WindParams& GetParams() const { return m_params; }

    Vec3 SampleWind(const Vec3& worldPos, float time) const;

private:
    WindParams m_params{};
};

} // namespace sphere::weather
