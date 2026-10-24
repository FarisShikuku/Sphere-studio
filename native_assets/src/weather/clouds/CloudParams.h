#pragma once
#include "scene/Transform.h"

namespace sphere::weather {

enum class CloudType {
    Stratus = 0,       // Flat, smooth low-altitude sheets
    Stratocumulus = 1, // Rolling fluffy/wave clouds
    Cumulus = 2        // Puffy cauliflower towering heaps
};

struct CloudParams {
    // Basic coverage & density
    float coverage = 0.55f;          // [0..1] weather map coverage multiplier
    float density = 1.0f;            // optical thickness multiplier
    CloudType type = CloudType::Cumulus; // Nubis height profile

    // Planetary Atmosphere Shell (Beckebans / Nubis model)
    float earthRadius = 700000.0f;        // meters
    float innerRadius = 705000.0f;        // base cloud altitude (earth + 5000m)
    float outerRadius = 722000.0f;        // top cloud altitude (inner + 17000m)

    // Lighting & Scattering (Beer-Lambert, Powder, Henyey-Greenstein)
    float forwardScatteringG1 = 0.80f;    // Silver lining forward peak
    float backwardScatteringG2 = -0.20f;  // Subtle backward scattering
    float powderEffect = 1.0f;            // Beer-Powder sugar approximation (1.0 - exp(-2*d))
    float sunAbsorption = 0.65f;          // Extinction on march to sun
    float ambientTop = 1.2f;              // Sky hemisphere ambient factor
    float ambientBottom = 0.45f;          // Earth bounce ambient factor

    // Noise & Detail Erosion (Perlin-Worley 3D)
    float cloudScale = 40.0f;             // Low-frequency Perlin-Worley spatial frequency
    float detailErosion = 0.45f;          // High-frequency 3D Worley erosion strength
    float windSpeed = 30.0f;              // km/h
    float cloudTopOffset = 750.0f;        // Altitude shear deflection along wind vector
    Vec3 windDirection{0.5f, 0.0f, 0.1f}; // Normalized wind drift direction

    // Raymarcher execution params
    int maxMarchingSteps = 64;            // Step count along camera ray
    int lightMarchingSteps = 6;           // Cone samples towards sun
    float stepSize = 120.0f;              // Adaptive primary step (meters)
};

} // namespace sphere::weather
