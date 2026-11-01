#include "CloudRaymarch.h"
#include <cmath>
#include <algorithm>

namespace sphere::weather {

namespace {
// Meteoros 6-cone sampling kernel for light extinction
constexpr float NOISE_KERNEL[6][3] = {
    { 0.38051305f,  0.92453449f, -0.02111345f},
    {-0.50625799f, -0.03590792f, -0.86163418f},
    {-0.32509218f, -0.94557439f,  0.01428793f},
    { 0.09026238f, -0.27376545f,  0.95755165f},
    { 0.28128598f,  0.42443639f, -0.86065785f},
    {-0.16852403f,  0.14748697f,  0.97460106f}
};

// Meteoros 4x4 Bayer matrix for ray offset jittering to eliminate stepping artifacts
constexpr float BAYER_4X4[4][4] = {
    { 0.0f / 16.0f,  8.0f / 16.0f,  2.0f / 16.0f, 10.0f / 16.0f},
    {12.0f / 16.0f,  4.0f / 16.0f, 14.0f / 16.0f,  6.0f / 16.0f},
    { 3.0f / 16.0f, 11.0f / 16.0f,  1.0f / 16.0f,  9.0f / 16.0f},
    {15.0f / 16.0f,  7.0f / 16.0f, 13.0f / 16.0f,  5.0f / 16.0f}
};

// Meteoros ambient illumination palette for sky / earth ambient bounce
constexpr Vec3 AMBIENT_TOP{0.68f, 0.62f, 0.64f};
constexpr Vec3 AMBIENT_BOTTOM{0.24f, 0.26f, 0.32f};
} // namespace

bool CloudRaymarch::IntersectAtmosphere(
    const Vec3& ro,
    const Vec3& rd,
    float innerRadius,
    float outerRadius,
    float earthRadius,
    float& outNear,
    float& outFar
) const {
    if (rd.y <= 0.005f) {
        return false;
    }

    // Flat earth approximation with curved horizon for performance
    float hInner = innerRadius - earthRadius;
    float hOuter = outerRadius - earthRadius;

    float tInner = (hInner - ro.y) / rd.y;
    float tOuter = (hOuter - ro.y) / rd.y;

    if (tOuter < 0.0f) return false;

    outNear = std::max(0.0f, tInner);
    outFar = tOuter;
    return outFar > outNear;
}

float CloudRaymarch::RaymarchToLight(
    const Vec3& pos,
    const Vec3& sunDir,
    const CloudGen& generator,
    float time,
    float stepSize
) const {
    float lightDensity = 0.0f;
    float ds = stepSize * 3.5f;

    for (int i = 0; i < 6; ++i) {
        float coneScale = (i + 1) * 0.5f;
        Vec3 samplePos{
            pos.x + (sunDir.x + NOISE_KERNEL[i][0] * 0.15f) * ds * coneScale,
            pos.y + (sunDir.y + NOISE_KERNEL[i][1] * 0.15f) * ds * coneScale,
            pos.z + (sunDir.z + NOISE_KERNEL[i][2] * 0.15f) * ds * coneScale
        };

        lightDensity += generator.SampleCloudDensity(samplePos, time, false) * ds;
    }

    return lightDensity;
}

CloudRayResult CloudRaymarch::March(
    const Vec3& origin,
    const Vec3& dir,
    const Vec3& sunDir,
    const Vec3& sunColor,
    const CloudGen& generator,
    float time,
    int pixelX,
    int pixelY
) const {
    CloudRayResult result;
    const auto& params = generator.GetParams();

    float tNear = 0.0f;
    float tFar = 0.0f;
    if (!IntersectAtmosphere(origin, dir, params.innerRadius, params.outerRadius, params.earthRadius, tNear, tFar)) {
        return result;
    }

    float marchDist = tFar - tNear;
    int steps = std::min(params.maxMarchingSteps, 64);
    float stepSize = marchDist / static_cast<float>(steps);

    // Compute phase function for directional sunlight scattering (silver lining forward peak)
    float cosTheta = dir.x * sunDir.x + dir.y * sunDir.y + dir.z * sunDir.z;
    float phaseSun = generator.DualPhaseHG(cosTheta);

    float transmittance = 1.0f;
    Vec3 accumulatedRadiance{0.0f, 0.0f, 0.0f};

    // Apply Meteoros 4x4 Bayer interleaved ray jitter
    float bayerOffset = 0.0f;
    if (params.enableBayerJitter) {
        bayerOffset = BAYER_4X4[pixelY & 3][pixelX & 3];
    }
    float t = tNear + bayerOffset * stepSize;

    for (int i = 0; i < steps; ++i) {
        if (transmittance < 0.01f) break;

        Vec3 p{origin.x + dir.x * t, origin.y + dir.y * t, origin.z + dir.z * t};
        float sampleDensity = generator.SampleCloudDensity(p, time, true);

        if (sampleDensity > 0.001f) {
            float opticalDepth = sampleDensity * stepSize * 0.005f;

            // 1. Meteoros Multi-Scattering Dual Beer-Lambert law
            float multiBeerAtten = generator.MultiBeer(opticalDepth, params.multiScatteringFactor);

            // 2. Powder sugar effect (AmanSachan1/Meteoros approximation)
            float powderAtten = generator.Powder(opticalDepth);
            float energyFactor = multiBeerAtten * (1.0f - (1.0f - powderAtten) * params.powderEffect);

            // 3. March towards sun to calculate cloud self-shadowing
            float lightDepth = RaymarchToLight(p, sunDir, generator, time, stepSize);
            float sunEnergy = generator.MultiBeer(lightDepth * params.sunAbsorption * 0.005f, params.multiScatteringFactor * 0.5f);

            // 4. Ambient light combination (sky hemisphere + ground bounce)
            float hFrac = generator.GetHeightFraction(p);
            Vec3 ambient = {
                (AMBIENT_BOTTOM.x * (1.0f - hFrac) + AMBIENT_TOP.x * hFrac) * params.ambientTop,
                (AMBIENT_BOTTOM.y * (1.0f - hFrac) + AMBIENT_TOP.y * hFrac) * params.ambientTop,
                (AMBIENT_BOTTOM.z * (1.0f - hFrac) + AMBIENT_TOP.z * hFrac) * params.ambientTop
            };

            // Direct sun contribution with dual-lobe Henyey-Greenstein silver lining
            Vec3 direct = {
                sunColor.x * sunEnergy * phaseSun,
                sunColor.y * sunEnergy * phaseSun,
                sunColor.z * sunEnergy * phaseSun
            };

            Vec3 stepLight = {
                (ambient.x + direct.x) * sampleDensity,
                (ambient.y + direct.y) * sampleDensity,
                (ambient.z + direct.z) * sampleDensity
            };

            accumulatedRadiance.x += stepLight.x * transmittance;
            accumulatedRadiance.y += stepLight.y * transmittance;
            accumulatedRadiance.z += stepLight.z * transmittance;

            transmittance *= energyFactor;
        }

        t += stepSize;
    }

    result.transmittance = transmittance;
    result.cloudAlpha = std::clamp(1.0f - transmittance, 0.0f, 1.0f);
    result.radiance = accumulatedRadiance;
    result.depthMeters = tNear;
    // Meteoros god-ray occlusion factor (1.0 = clear sunlight beam, 0.0 = occluded by thick cloud)
    result.godRayOcclusion = transmittance;
    return result;
}

} // namespace sphere::weather
