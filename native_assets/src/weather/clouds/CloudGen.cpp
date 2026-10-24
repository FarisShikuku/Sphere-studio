#include "CloudGen.h"
#include <cmath>
#include <algorithm>

namespace sphere::weather {

namespace {
// Beckebans Nubis height gradients for distinct cloud species
// vec4(bottom_fade_start, bottom_fade_end, top_fade_start, top_fade_end)
constexpr float STRATUS_GRADIENT[4] = {0.0f, 0.10f, 0.20f, 0.30f};
constexpr float STRATOCUMULUS_GRADIENT[4] = {0.02f, 0.20f, 0.48f, 0.625f};
constexpr float CUMULUS_GRADIENT[4] = {0.00f, 0.1625f, 0.88f, 0.98f};

float Smoothstep(float edge0, float edge1, float x) {
    float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}
} // namespace

float CloudGen::GetHeightFraction(const Vec3& worldPos) const {
    // Relative height within planetary atmosphere shell
    // sphereCenter is placed at (0, -earthRadius, 0)
    float altitude = worldPos.y;
    return std::clamp((altitude - (m_params.innerRadius - m_params.earthRadius)) / 
                      (m_params.outerRadius - m_params.innerRadius), 0.0f, 1.0f);
}

float CloudGen::Remap(float val, float inMin, float inMax, float outMin, float outMax) const {
    if (std::abs(inMax - inMin) < 1e-6f) return outMin;
    return outMin + ((val - inMin) / (inMax - inMin)) * (outMax - outMin);
}

float CloudGen::GetDensityForCloud(float heightFraction, CloudType type) const {
    const float* grad = nullptr;
    switch (type) {
        case CloudType::Stratus:
            grad = STRATUS_GRADIENT;
            break;
        case CloudType::Stratocumulus:
            grad = STRATOCUMULUS_GRADIENT;
            break;
        case CloudType::Cumulus:
        default:
            grad = CUMULUS_GRADIENT;
            break;
    }

    // Nubis Decima smoothstep height gradient curve
    float bottomFade = Smoothstep(grad[0], grad[1], heightFraction);
    float topFade = 1.0f - Smoothstep(grad[2], grad[3], heightFraction);
    return std::max(0.0f, bottomFade * topFade);
}

float CloudGen::Beer(float opticalDepth) const {
    return std::exp(-opticalDepth);
}

float CloudGen::Powder(float opticalDepth) const {
    // Robert Beckebans powder sugar effect: highlights outer edges, darkens dense cores
    return 1.0f - std::exp(-2.0f * opticalDepth);
}

float CloudGen::HenyeyGreenstein(float cosTheta, float g) const {
    float gg = g * g;
    float denom = 1.0f + gg - 2.0f * g * cosTheta;
    if (denom < 1e-4f) denom = 1e-4f;
    return (1.0f - gg) / (4.0f * 3.14159265f * std::pow(denom, 1.5f));
}

float CloudGen::DualPhaseHG(float cosTheta) const {
    // Dual-lobe Henyey-Greenstein: forward peak for silver lining + subtle backscattering
    float forward = HenyeyGreenstein(cosTheta, m_params.forwardScatteringG1);
    float backward = HenyeyGreenstein(cosTheta, m_params.backwardScatteringG2);
    return forward * 0.7f + backward * 0.3f;
}

float CloudGen::SampleCloudDensity(const Vec3& p, float time, bool expensive) const {
    float heightFraction = GetHeightFraction(p);
    if (heightFraction <= 0.0f || heightFraction >= 1.0f) {
        return 0.0f;
    }

    // Wind advection animation & height shear offset (Beckebans Nubis wind model)
    Vec3 wind = m_params.windDirection;
    float shear = heightFraction * m_params.cloudTopOffset;
    float drift = time * (m_params.windSpeed * (1000.0f / 3600.0f) * 0.2f);

    float sx = (p.x + wind.x * (shear + drift)) * 0.0003f * m_params.cloudScale;
    float sz = (p.z + wind.z * (shear + drift)) * 0.0003f * m_params.cloudScale;
    float sy = heightFraction * 4.0f;

    // 1. Low-frequency 3D Perlin-Worley FBM base
    float w1 = std::sin(sx * 1.5f) * std::cos(sz * 1.5f + sy * 0.8f);
    float w2 = std::sin(sx * 3.1f + 1.2f) * std::cos(sz * 3.1f + sy * 1.6f);
    float w3 = std::sin(sx * 6.2f + 2.5f) * std::cos(sz * 6.2f + sy * 3.2f);
    float lowFreqFBM = (w1 * 0.625f + w2 * 0.250f + w3 * 0.125f) * 0.5f + 0.5f;

    // 2. Weather map coverage & height density attenuation
    float baseCloud = Remap(lowFreqFBM, -(1.0f - lowFreqFBM), 1.0f, 0.0f, 1.0f);
    float typeDensity = GetDensityForCloud(heightFraction, m_params.type);
    baseCloud *= typeDensity;

    float coverage = m_params.coverage;
    float baseCloudWithCoverage = Remap(baseCloud, 1.0f - coverage, 1.0f, 0.0f, 1.0f);
    baseCloudWithCoverage = std::clamp(baseCloudWithCoverage * coverage, 0.0f, 1.0f);

    // 3. High-frequency 3D Worley noise erosion
    if (expensive && m_params.detailErosion > 0.01f && baseCloudWithCoverage > 0.01f) {
        float ex = (p.x - wind.x * drift * 0.5f) * 0.0012f * m_params.cloudScale;
        float ez = (p.z - wind.z * drift * 0.5f) * 0.0012f * m_params.cloudScale;
        float ey = heightFraction * 12.0f;

        float hf1 = std::abs(std::sin(ex * 3.0f) * std::sin(ez * 3.0f + ey));
        float hf2 = std::abs(std::cos(ex * 6.0f) * std::cos(ez * 6.0f + ey * 2.0f));
        float highFreqFBM = hf1 * 0.625f + hf2 * 0.375f;

        // Transition modifier: erode more at top & bottom edges
        float noiseModifier = std::clamp(heightFraction * 8.0f, 0.0f, 1.0f);
        float highFreqModifier = highFreqFBM * (1.0f - noiseModifier) + (1.0f - highFreqFBM) * noiseModifier;

        baseCloudWithCoverage = baseCloudWithCoverage - highFreqModifier * m_params.detailErosion * (1.0f - baseCloudWithCoverage);
        baseCloudWithCoverage = Remap(baseCloudWithCoverage * 2.0f, highFreqModifier * 0.2f, 1.0f, 0.0f, 1.0f);
    }

    return std::clamp(baseCloudWithCoverage * m_params.density, 0.0f, 1.0f);
}

} // namespace sphere::weather
