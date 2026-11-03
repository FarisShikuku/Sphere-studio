#pragma once
#include "Transform.h"
#include <cmath>
namespace sphere {
/**
 * Atmospheric Sky & Sun Model referencing kentril0/WaterSurfaceRendering and Preetham/Hosek-Wilkie.
 * Provides realistic Rayleigh & Mie scattering, dynamic solar positioning, and corona glow.
 */
class SkyModel {
public:
    // Toggles for scene rendering
    bool enableSky = true;
    bool enableSun = true;
    // Solar coordinates
    float sunElevation = 35.0f; // degrees above horizon (0 = sunrise/sunset, 90 = zenith)
    float sunAzimuth = 135.0f;   // degrees around horizon (0 = North, 90 = East, etc.)
    // Atmospheric parameters
    float turbidity = 2.5f;     // Aerosol / haze density (2.0 = crystal clear, 8.0 = hazy)
    float groundAlbedo = 0.12f;  // Earth surface reflectance
    // Derived properties
    Vec3 sunDirection{-0.4f, 0.7f, -0.6f};
    Vec3 sunColor{1.0f, 0.95f, 0.85f};
    float sunIntensity = 22.0f;
    float sunAngularRadius = 0.045f;
    SkyModel();
    void SetSunPosition(float elevationDeg, float azimuthDeg);
    void UpdateSunDirection();
    // Evaluates atmospheric radiance along view direction
    Vec3 EvaluateSkyColor(const Vec3& viewDir) const;
    // Evaluates direct solar disk and Mie corona flare
    Vec3 EvaluateSunDisk(const Vec3& viewDir) const;
    // Computes sun illumination color based on atmospheric extinction
    Vec3 ComputeSunTransmittance() const;
};
} // namespace sphere