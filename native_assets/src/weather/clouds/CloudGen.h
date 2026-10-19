#pragma once
#include "CloudParams.h"
#include "scene/Transform.h"

namespace sphere::weather {

class CloudGen {
public:
    CloudGen() = default;
    void SetParams(const CloudParams& params) { m_params = params; }
    const CloudParams& GetParams() const { return m_params; }

    float SampleCloudDensity(const Vec3& p, float time) const;

private:
    CloudParams m_params{};
};

} // namespace sphere::weather
