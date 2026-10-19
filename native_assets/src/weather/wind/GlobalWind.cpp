#include "GlobalWind.h"
#include <cmath>

namespace sphere::weather {

Vec3 GlobalWind::SampleWind(const Vec3& worldPos, float time) const {
    float rad = m_params.azimuthDegrees * (3.14159265f / 180.0f);
    float baseDirX = std::cos(rad);
    float baseDirZ = std::sin(rad);

    float gust = std::sin(time * m_params.gustFrequency * 6.28318f) * m_params.gustMagnitude;
    float currentSpeed = m_params.baseSpeed + gust;

    float turbX = std::sin(worldPos.z * m_params.turbulenceScale + time) * 0.15f;
    float turbZ = std::cos(worldPos.x * m_params.turbulenceScale + time) * 0.15f;

    return Vec3{(baseDirX + turbX) * currentSpeed, 0.0f, (baseDirZ + turbZ) * currentSpeed};
}

} // namespace sphere::weather
