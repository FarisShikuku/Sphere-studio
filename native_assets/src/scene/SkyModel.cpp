#include "SkyModel.h"
#include <algorithm>

namespace sphere {

SkyModel::SkyModel() {
    UpdateSunDirection();
}

void SkyModel::SetSunPosition(float elevationDeg, float azimuthDeg) {
    sunElevation = elevationDeg;
    sunAzimuth = azimuthDeg;
    UpdateSunDirection();
}

void SkyModel::UpdateSunDirection() {
    float elRad = sunElevation * 0.01745329251f; // deg to rad
    float azRad = sunAzimuth * 0.01745329251f;

    // Y is up, X is East, Z is North
    sunDirection.x = std::cos(elRad) * std::sin(azRad);
    sunDirection.y = std::sin(elRad);
    sunDirection.z = std::cos(elRad) * std::cos(azRad);

    // Normalize
    float len = std::sqrt(sunDirection.x * sunDirection.x + sunDirection.y * sunDirection.y + sunDirection.z * sunDirection.z);
    if (len > 0.0001f) {
        sunDirection.x /= len;
        sunDirection.y /= len;
        sunDirection.z /= len;
    }

    sunColor = ComputeSunTransmittance();
}

Vec3 SkyModel::ComputeSunTransmittance() const {
    // Atmospheric extinction curve
    float sinEl = std::max(0.01f, sunDirection.y);
    float airMass = 1.0f / (sinEl + 0.15f * std::pow(sunElevation + 3.885f, -1.253f));

    // Rayleigh optical depth per channel (R, G, B)
    float tauR_r = 0.065f * airMass;
    float tauR_g = 0.130f * airMass;
    float tauR_b = 0.280f * airMass;

    // Mie aerosol optical depth
    float tauM = 0.08f * (turbidity - 1.0f) * airMass;

    Vec3 trans;
    trans.x = std::exp(-(tauR_r + tauM));
    trans.y = std::exp(-(tauR_g + tauM));
    trans.z = std::exp(-(tauR_b + tauM));

    return trans;
}

Vec3 SkyModel::EvaluateSkyColor(const Vec3& viewDir) const {
    if (!enableSky) {
        return Vec3{0.04f, 0.05f, 0.07f}; // Deep studio dark fallback
    }

    float cosTheta = std::max(0.0f, viewDir.y);
    float cosGamma = viewDir.x * sunDirection.x + viewDir.y * sunDirection.y + viewDir.z * sunDirection.z;

    // Zenith to horizon Rayleigh gradient (Preetham / Hosek approximation)
    Vec3 zenithColor{0.18f, 0.42f, 0.85f};
    Vec3 horizonColor{0.65f, 0.75f, 0.88f};

    // Sunset/sunrise horizon warm shift
    if (sunElevation < 25.0f) {
        float sunsetFactor = std::max(0.0f, 1.0f - sunElevation / 25.0f);
        horizonColor.x = horizonColor.x * (1.0f - sunsetFactor) + 0.95f * sunsetFactor;
        horizonColor.y = horizonColor.y * (1.0f - sunsetFactor) + 0.52f * sunsetFactor;
        horizonColor.z = horizonColor.z * (1.0f - sunsetFactor) + 0.22f * sunsetFactor;
    }

    // Blend from horizon to zenith
    float heightBlend = std::pow(cosTheta, 0.45f);
    Vec3 sky;
    sky.x = horizonColor.x * (1.0f - heightBlend) + zenithColor.x * heightBlend;
    sky.y = horizonColor.y * (1.0f - heightBlend) + zenithColor.y * heightBlend;
    sky.z = horizonColor.z * (1.0f - heightBlend) + zenithColor.z * heightBlend;

    // Mie forward scattering haze around sun
    if (enableSun && cosGamma > 0.0f) {
        float miePhase = std::pow(cosGamma, 16.0f) * 0.45f;
        sky.x += sunColor.x * miePhase;
        sky.y += sunColor.y * miePhase * 0.85f;
        sky.z += sunColor.z * miePhase * 0.65f;
    }

    return sky;
}

Vec3 SkyModel::EvaluateSunDisk(const Vec3& viewDir) const {
    if (!enableSun) return Vec3{0.0f, 0.0f, 0.0f};

    float cosAngle = viewDir.x * sunDirection.x + viewDir.y * sunDirection.y + viewDir.z * sunDirection.z;
    float threshold = std::cos(sunAngularRadius);

    if (cosAngle > threshold) {
        // Core solar disk
        return Vec3{sunColor.x * sunIntensity, sunColor.y * sunIntensity, sunColor.z * sunIntensity};
    } else if (cosAngle > threshold - 0.005f) {
        // Limb darkening & anti-aliased edge
        float edge = (cosAngle - (threshold - 0.005f)) / 0.005f;
        return Vec3{sunColor.x * sunIntensity * edge, sunColor.y * sunIntensity * edge, sunColor.z * sunIntensity * edge};
    }

    return Vec3{0.0f, 0.0f, 0.0f};
}

} // namespace sphere