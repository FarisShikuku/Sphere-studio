#pragma once
// ============================================================================
// WindField.h - 3D Curl-Noise Turbulent Wind Simulator in C++17 / C
// ============================================================================

#include "WindParams.h"
#include "../common/VegetationMath.h"

#ifdef __cplusplus
extern "C" {
#endif

// Plain C 3D wind query
VegVec3_C veg_sample_wind_velocity(
    float worldX, float worldY, float worldZ,
    float timeSec,
    float baseSpeedMps,
    float windDirectionRad,
    float gustFreq,
    float gustAmp
);

#ifdef __cplusplus
} // extern "C"
#endif

namespace vegetation {

class WindField {
public:
    explicit WindField(const WindSynthesisParams& params = WindSynthesisParams{});

    Vec3 sampleWindVelocity(float worldX, float worldY, float worldZ, float timeSec) const;

    void setParams(const WindSynthesisParams& p) { m_params = p; }
    const WindSynthesisParams& getParams() const { return m_params; }

private:
    WindSynthesisParams m_params;
};

} // namespace vegetation
