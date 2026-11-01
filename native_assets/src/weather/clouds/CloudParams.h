#pragma once
#include "scene/Transform.h"

namespace sphere::weather {

// Meteoros cloud species height profiles
enum class CloudType {
    Stratus = 0,       // Flat, smooth low-altitude sheets
    Stratocumulus = 1, // Rolling fluffy/wave clouds
    Cumulus = 2,       // Puffy cauliflower towering heaps
    Cumulonimbus = 3   // Massive anvil thunderstorm clouds
};

struct CloudParams {
    // Basic coverage & density
    float coverage = 0.55f;          // [0..1] weather map coverage multiplier
    float density = 1.0f;            // optical thickness multiplier
    CloudType type = CloudType::Cumulus; // Height profile species

    // Planetary Atmosphere Shell (Meteoros / Decima model)
    float earthRadius = 700000.0f;        // meters
    float innerRadius = 705000.0f;        // base cloud altitude (earth + 5000m)
    float outerRadius = 722000.0f;        // top cloud altitude (inner + 17000m)

    // Lighting & Scattering (AmanSachan1/Meteoros Multi-Scattering & Dual HG)
    float forwardScatteringG1 = 0.82f;    // Silver lining forward peak
    float backwardScatteringG2 = -0.22f;  // Subtle backward scattering
    float powderEffect = 1.0f;            // Beer-Powder sugar approximation (1.0 - exp(-2*d))
    float multiScatteringFactor = 0.50f;  // Meteoros Dual Beer-Lambert multi-scattering factor
    float sunAbsorption = 0.65f;          // Extinction on march to sun
    float ambientTop = 1.2f;              // Sky hemisphere ambient factor
    float ambientBottom = 0.45f;          // Earth bounce ambient factor

    // Noise & Detail Erosion (Perlin-Worley 3D)
    float cloudScale = 40.0f;             // Low-frequency Perlin-Worley spatial frequency
    float detailErosion = 0.45f;          // High-frequency 3D Worley erosion strength
    float windSpeed = 30.0f;              // km/h
    float cloudTopOffset = 750.0f;        // Altitude shear deflection along wind vector
    Vec3 windDirection{0.5f, 0.0f, 0.1f}; // Normalized wind drift direction

    // Volumetric Crepuscular God Rays (AmanSachan1/Meteoros)
    bool enableGodRays = true;
    float godRaysIntensity = 0.65f;
    float godRaysDecay = 0.96f;
    float godRaysDensity = 0.85f;
    float godRaysWeight = 0.58f;
    int godRaysSamples = 32;

    // Raymarcher execution params & Bayer Jitter (Anti-Aliasing)
    int maxMarchingSteps = 64;            // Step count along camera ray
    int lightMarchingSteps = 6;           // Cone samples towards sun
    float stepSize = 120.0f;              // Adaptive primary step (meters)
    bool enableBayerJitter = true;        // 4x4 Bayer interleaved ray dither against slicing
    float toneMapExposure = 1.2f;         // ACES HDR exposure multiplier
};

} // namespace sphere::weather
