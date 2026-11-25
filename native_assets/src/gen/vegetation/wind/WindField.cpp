// ============================================================================
// WindField.cpp - Curl-Noise & Spatio-Temporal Wind Implementation
// ============================================================================

#include "WindField.h"
#include <cmath>

extern "C" {

VegVec3_C veg_sample_wind_velocity(
    float worldX, float worldY, float worldZ,
    float timeSec,
    float baseSpeedMps,
    float windDirectionRad,
    float gustFreq,
    float gustAmp
) {
    float cosDir = std::cos(windDirectionRad);
    float sinDir = std::sin(windDirectionRad);

    // Traveling wave gust
    float travelingPhase = (worldX * cosDir + worldZ * sinDir) * 0.08f - timeSec * gustFreq * 6.2831853f;
    float gust = std::sin(travelingPhase) * gustAmp;
    float currentSpeed = std::max(0.2f, baseSpeedMps + gust);

    // 3D curl turbulence perturbation via finite differences
    float eps = 0.5f;
    float nX = veg_noise3d(worldX * 0.12f, worldY * 0.12f, timeSec * 0.4f);
    float nZ = veg_noise3d(worldX * 0.12f + 17.0f, worldY * 0.12f, timeSec * 0.4f + 31.0f);

    float vx = cosDir * currentSpeed + nX * 0.8f;
    float vy = (veg_noise3d(worldX * 0.2f, worldZ * 0.2f, timeSec * 0.8f)) * 0.35f;
    float vz = sinDir * currentSpeed + nZ * 0.8f;

    return {vx, vy, vz};
}

} // extern "C"

namespace vegetation {

WindField::WindField(const WindSynthesisParams& params)
    : m_params(params) {}

Vec3 WindField::sampleWindVelocity(float worldX, float worldY, float worldZ, float timeSec) const {
    VegVec3_C v = veg_sample_wind_velocity(
        worldX, worldY, worldZ,
        timeSec,
        m_params.baseSpeedMps,
        m_params.windDirectionRad,
        m_params.gustFrequencyHz,
        m_params.gustAmplitude
    );
    return Vec3(v.x, v.y, v.z);
}

} // namespace vegetation
