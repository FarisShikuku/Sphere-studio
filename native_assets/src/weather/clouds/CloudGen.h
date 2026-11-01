#pragma once
#include "CloudParams.h"
#include "scene/Transform.h"

namespace sphere::weather {

class CloudGen {
public:
    CloudGen() = default;
    void SetParams(const CloudParams& params) { m_params = params; }
    const CloudParams& GetParams() const { return m_params; }

    // Meteoros Nubis height & gradient functions
    float GetHeightFraction(const Vec3& worldPos) const;
    float GetDensityForCloud(float heightFraction, CloudType type) const;
    float Remap(float val, float inMin, float inMax, float outMin, float outMax) const;

    // Optical functions (Beer's Law, MultiBeer, Powder effect, Henyey-Greenstein)
    float Beer(float opticalDepth) const;
    float MultiBeer(float opticalDepth, float multiScatteringFactor) const;
    float Powder(float opticalDepth) const;
    float HenyeyGreenstein(float cosTheta, float g) const;
    float DualPhaseHG(float cosTheta) const;

    // 3D Procedural density evaluation
    float SampleCloudDensity(const Vec3& p, float time, bool expensive = true) const;

private:
    CloudParams m_params{};
};

} // namespace sphere::weather
