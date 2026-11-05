#include "Atmosphere.h"
#include <algorithm>

namespace sphere::lighting {

Atmosphere::Atmosphere() {
    UpdateAtmosphere();
}

void Atmosphere::SetSunPosition(float azimuthDeg, float elevationDeg) {
    m_sunAzimuthDeg = azimuthDeg;
    m_sunElevationDeg = std::clamp(elevationDeg, -10.0f, 90.0f);

    constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;
    float azRad = m_sunAzimuthDeg * kDegToRad;
    float elRad = m_sunElevationDeg * kDegToRad;

    m_sunDirection = Vec3{
        std::sin(azRad) * std::cos(elRad),
        std::sin(elRad),
        -std::cos(azRad) * std::cos(elRad)
    }.Normalized();

    UpdateAtmosphere();
}

void Atmosphere::UpdateAtmosphere() {
    m_sunTransmittance = ComputeSunTransmittance();
    m_skyDiffuseIrradiance = ComputeSkyDiffuseIrradiance();
}

Vec3 Atmosphere::ComputeSunTransmittance() const {
    // Air mass calculation via Kasten-Young formula
    float zenithAngle = std::max(0.0f, 90.0f - m_sunElevationDeg);
    constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;
    float zenithRad = zenithAngle * kDegToRad;

    if (m_sunElevationDeg < -2.0f) {
        return Vec3{0.0f, 0.0f, 0.0f}; // Below horizon
    }

    // Optical air mass
    float cosZ = std::max(0.01f, std::cos(zenithRad));
    float airMass = 1.0f / (cosZ + 0.50572f * std::pow(96.07995f - zenithAngle, -1.6364f));
    airMass = std::clamp(airMass, 1.0f, 40.0f);

    // Optical depths for Rayleigh, Mie, and Ozone
    // Scaled by turbidity
    Vec3 betaR = m_rayleighCoeffs * m_rayleighScaleHeight;
    Vec3 betaM = m_mieCoeffs * m_mieScaleHeight * (m_turbidity * 0.4f);
    Vec3 betaO = m_ozoneCoeffs * 4000.0f; // Effective ozone layer depth

    Vec3 opticalDepthTotal = (betaR + betaM + betaO) * airMass;

    // Beer-Lambert transmittance: T = exp(-tau)
    return Vec3{
        std::exp(-opticalDepthTotal.x),
        std::exp(-opticalDepthTotal.y),
        std::exp(-opticalDepthTotal.z)
    };
}

Vec3 Atmosphere::ComputeDirectSunIlluminance() const {
    Vec3 trans = m_sunTransmittance;
    float sinElevation = std::max(0.0f, std::sin(m_sunElevationDeg * 3.14159265f / 180.0f));
    float directLux = m_solarIlluminance * sinElevation;
    return trans * directLux;
}

Vec3 Atmosphere::ComputeSkyDiffuseIrradiance() const {
    // Diffuse sky radiance approximation integrating hemispherical single-scattering
    float sinElevation = std::max(0.05f, std::sin(m_sunElevationDeg * 3.14159265f / 180.0f));
    float skyLux = m_solarIlluminance * 0.18f * std::pow(sinElevation, 0.7f);

    // Rayleigh scattered diffuse sky has characteristic deep blue / cyan tint
    Vec3 skyTint = Vec3{0.32f, 0.55f, 0.95f} + m_groundAlbedo * 0.2f;
    return skyTint * (skyLux / 10000.0f);
}

GpuAtmosphere Atmosphere::BuildGpuAtmosphere() const {
    GpuAtmosphere gpu{};
    float zenithRad = (90.0f - m_sunElevationDeg) * 3.14159265f / 180.0f;

    gpu.sunDirectionAndZenith = Vec4{m_sunDirection.x, m_sunDirection.y, m_sunDirection.z, zenithRad};

    Vec3 trans = m_sunTransmittance;
    Vec3 directIllum = ComputeDirectSunIlluminance();
    gpu.sunColorAndIntensity = Vec4{trans.x, trans.y, trans.z, directIllum.Length()};

    gpu.rayleighScattering = Vec4{m_rayleighCoeffs.x, m_rayleighCoeffs.y, m_rayleighCoeffs.z, m_rayleighScaleHeight};
    gpu.mieScattering = Vec4{m_mieCoeffs.x * m_turbidity, m_mieCoeffs.y * m_turbidity, m_mieCoeffs.z * m_turbidity, m_mieScaleHeight};
    gpu.ozoneAbsorption = Vec4{m_ozoneCoeffs.x, m_ozoneCoeffs.y, m_ozoneCoeffs.z, m_mieAsymmetry};

    gpu.groundAlbedoAndAtmosphereRadius = Vec4{m_groundAlbedo.x, m_groundAlbedo.y, m_groundAlbedo.z, 6420000.0f}; // Earth radius + 60km atmosphere
    gpu.skyAmbientHemisphere = Vec4{m_skyDiffuseIrradiance.x, m_skyDiffuseIrradiance.y, m_skyDiffuseIrradiance.z, m_sunAngularRadiusRad};

    return gpu;
}

} // namespace sphere::lighting
