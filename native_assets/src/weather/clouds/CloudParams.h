#pragma once
#include "scene/Transform.h"

namespace sphere::weather {

// Meteoros cloud species height profiles
enum class CloudType {
    Stratus = 0,       // Flat, smooth low-altitude sheets (1000m - 2000m)
    Stratocumulus = 1, // Rolling fluffy/wave clouds (800m - 2500m)
    Cumulus = 2,       // Puffy cauliflower towering heaps (1200m - 4000m)
    Cumulonimbus = 3,  // Massive anvil thunderstorm clouds (1000m - 12000m)
    Altocumulus = 4,   // Mid-altitude mottled patches and rolls (2500m - 5000m)
    Cirrus = 5         // High-altitude wispy ice-crystal filaments (6000m - 10000m)
};

struct CloudParams {
    // Basic coverage & density
    float coverage = 0.65f;          // [0..1] weather map coverage multiplier
    float density = 1.2f;            // optical thickness multiplier
    CloudType type = CloudType::Stratocumulus; // Height profile species

    // Planetary Atmosphere Shell (Meteoros / Decima model)
    float earthRadius = 6371000.0f;       // Earth mean radius (meters)
    float innerRadius = 6372500.0f;       // Cloud base altitude (+1500m)
    float outerRadius = 6376500.0f;       // Cloud top altitude (+5500m)

    // Lighting & Scattering (AmanSachan1/Meteoros Multi-Scattering & Dual HG)
    float forwardScatteringG1 = 0.82f;    // Silver lining forward peak
    float backwardScatteringG2 = -0.22f;  // Subtle backward scattering
    float powderEffect = 0.65f;           // Beer-Powder sugar approximation (1.0 - exp(-2*d))
    float multiScatteringFactor = 0.55f;  // Meteoros Dual Beer-Lambert multi-scattering factor
    float sunAbsorption = 0.65f;          // Extinction on march to sun
    float ambientTop = 1.35f;             // Sky hemisphere ambient factor
    float ambientBottom = 0.40f;          // Earth bounce ambient factor

    // Noise & Detail Erosion (Perlin-Worley 3D)
    float cloudScale = 1.0f;              // Spatial frequency scale multiplier
    float detailErosion = 0.35f;          // High-frequency 3D Worley erosion strength
    float windSpeed = 24.0f;              // km/h
    float cloudTopOffset = 450.0f;        // Altitude shear deflection along wind vector
    Vec3 windDirection{0.707f, 0.0f, 0.707f}; // Normalized wind drift direction

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
