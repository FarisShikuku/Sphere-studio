#pragma once
#include "scene/Transform.h"
#include <cmath>

namespace sphere::lighting {

/**
 * Atmosphere - Physically Based Sky & Sun Illuminance Model
 * Directly referencing Gaukler/PlainRenderer physical atmosphere architecture.
 *
 * Simulates Rayleigh scattering (air molecules), Mie aerosol scattering (haze/dust),
 * Henyey-Greenstein / Cornette-Shanks phase functions, solar disk irradiance,
 * and ozone absorption band for true physical sun and sky radiance.
 */

// GPU std430 uniform block for atmosphere & sky pass
struct alignas(16) GpuAtmosphere {
    Vec4 sunDirectionAndZenith;    // xyz = sun direction, w = solar zenith angle
    Vec4 sunColorAndIntensity;     // xyz = sunlight RGB after atmospheric transmission, w = illuminance in lux
    Vec4 rayleighScattering;       // xyz = beta_R scattering coefficients (m^-1), w = scale height H_R (m)
    Vec4 mieScattering;            // xyz = beta_M scattering coefficients (m^-1), w = scale height H_M (m)
    Vec4 ozoneAbsorption;          // xyz = beta_O absorption coefficients (m^-1), w = mie asymmetry factor g
    Vec4 groundAlbedoAndAtmosphereRadius; // xyz = ground albedo RGB, w = planet radius / atmosphere max
    Vec4 skyAmbientHemisphere;     // xyz = diffuse sky irradiance on ground, w = sun disk angular radius rad
};

class Atmosphere {
public:
    Atmosphere();
    ~Atmosphere() = default;

    // Sun orientation
    void SetSunPosition(float azimuthDeg, float elevationDeg);
    float GetSunAzimuth() const { return m_sunAzimuthDeg; }
    float GetSunElevation() const { return m_sunElevationDeg; }
    Vec3 GetSunDirection() const { return m_sunDirection; }

    // Physical atmosphere properties
    void SetTurbidity(float turbidity) { m_turbidity = std::clamp(turbidity, 1.0f, 10.0f); UpdateAtmosphere(); }
    float GetTurbidity() const { return m_turbidity; }

    void SetGroundAlbedo(const Vec3& albedo) { m_groundAlbedo = albedo; UpdateAtmosphere(); }
    Vec3 GetGroundAlbedo() const { return m_groundAlbedo; }

    void SetMieAsymmetry(float g) { m_mieAsymmetry = std::clamp(g, 0.5f, 0.99f); UpdateAtmosphere(); }
    float GetMieAsymmetry() const { return m_mieAsymmetry; }

    // Compute transmitted sun color and direct illuminance at sea level
    Vec3 ComputeSunTransmittance() const;
    Vec3 ComputeDirectSunIlluminance() const; // In lux
    Vec3 ComputeSkyDiffuseIrradiance() const; // Hemispherical indirect sky irradiance

    // Build Vulkan GPU uniform buffer
    GpuAtmosphere BuildGpuAtmosphere() const;

private:
    void UpdateAtmosphere();

    float m_sunAzimuthDeg = 145.0f;
    float m_sunElevationDeg = 38.0f;
    Vec3 m_sunDirection{0.5f, 0.6f, -0.6f};

    float m_turbidity = 2.45f;            // 1.0 = pristine mountain air, 2.5 = clear continental, 6.0+ = hazy/industrial
    float m_mieAsymmetry = 0.76f;         // Henyey-Greenstein g factor
    float m_solarIlluminance = 120000.0f; // Direct solar illuminance outside atmosphere (lux)
    float m_sunAngularRadiusRad = 0.00465f; // ~0.53 degrees

    Vec3 m_rayleighCoeffs{5.802e-6f, 13.558e-6f, 33.1e-6f}; // Standard air scattering coefficients
    float m_rayleighScaleHeight = 8000.0f; // 8 km scale height

    Vec3 m_mieCoeffs{3.996e-6f, 3.996e-6f, 3.996e-6f};       // Aerosol scattering coefficients
    float m_mieScaleHeight = 1200.0f;      // 1.2 km scale height

    Vec3 m_ozoneCoeffs{0.650e-6f, 1.881e-6f, 0.085e-6f};     // Ozone absorption in Chappuis band
    Vec3 m_groundAlbedo{0.15f, 0.15f, 0.15f};

    // Cached runtime values
    Vec3 m_sunTransmittance{1.0f, 1.0f, 1.0f};
    Vec3 m_skyDiffuseIrradiance{0.15f, 0.20f, 0.32f};
};

} // namespace sphere::lighting
