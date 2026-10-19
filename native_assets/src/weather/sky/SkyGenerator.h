#pragma once
#include "WeatherSkyParams.h"
#include "scene/Transform.h"

namespace sphere::weather {

class SkyGenerator {
public:
    SkyGenerator() = default;
    void SetParams(const WeatherSkyParams& params) { m_params = params; }
    const WeatherSkyParams& GetParams() const { return m_params; }

    Vec3 ComputeSkyRadiance(const Vec3& viewDir, const Vec3& sunDir, float stormFactor) const;

private:
    WeatherSkyParams m_params{};
};

} // namespace sphere::weather
