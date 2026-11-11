#include "CloudGen.h"
#include <cmath>
#include <algorithm>

namespace sphere::weather {

namespace {
// Meteoros / Decima height gradients for distinct cloud species
// vec4(bottom_fade_start, bottom_fade_end, top_fade_start, top_fade_end)
constexpr float STRATUS_GRADIENT[4] = {0.0f, 0.08f, 0.22f, 0.35f};
constexpr float STRATOCUMULUS_GRADIENT[4] = {0.02f, 0.16f, 0.55f, 0.72f};
constexpr float CUMULUS_GRADIENT[4] = {0.00f, 0.14f, 0.80f, 0.98f};
constexpr float CUMULONIMBUS_GRADIENT[4] = {0.00f, 0.08f, 0.92f, 1.00f};
constexpr float ALTOCUMULUS_GRADIENT[4] = {0.35f, 0.48f, 0.75f, 0.88f};
constexpr float CIRRUS_GRADIENT[4] = {0.70f, 0.82f, 0.96f, 1.00f};

float Smoothstep(float edge0, float edge1, float x) {
    if (std::abs(edge1 - edge0) < 1e-6f) return x >= edge1 ? 1.0f : 0.0f;
    float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

// Fast analytical 3D hash & value noise for C++ real-time volumetric raymarching
float Hash3D(int x, int y, int z) {
    int n = x * 374761393 + y * 668265263 + z * 127412617;
    n = (n ^ (n >> 13)) * 127412617;
    return static_cast<float>(n & 0x7FFFFFFF) / static_cast<float>(0x7FFFFFFF);
}

float ValueNoise3D(float x, float y, float z) {
    int ix = static_cast<int>(std::floor(x));
    int iy = static_cast<int>(std::floor(y));
    int iz = static_cast<int>(std::floor(z));

    float fx = x - static_cast<float>(ix);
    float fy = y - static_cast<float>(iy);
    float fz = z - static_cast<float>(iz);

    // Quintic Hermite interpolant for smooth derivatives
    float u = fx * fx * fx * (fx * (fx * 6.0f - 15.0f) + 10.0f);
    float v = fy * fy * fy * (fy * (fy * 6.0f - 15.0f) + 10.0f);
    float w = fz * fz * fz * (fz * (fz * 6.0f - 15.0f) + 10.0f);

    float c000 = Hash3D(ix, iy, iz);
    float c100 = Hash3D(ix + 1, iy, iz);
    float c010 = Hash3D(ix, iy + 1, iz);
    float c110 = Hash3D(ix + 1, iy + 1, iz);
    float c001 = Hash3D(ix, iy, iz + 1);
    float c101 = Hash3D(ix + 1, iy, iz + 1);
    float c011 = Hash3D(ix, iy + 1, iz + 1);
    float c111 = Hash3D(ix + 1, iy + 1, iz + 1);

    float x00 = c000 + u * (c100 - c000);
    float x10 = c010 + u * (c110 - c010);
    float x01 = c001 + u * (c101 - c001);
    float x11 = c011 + u * (c111 - c011);

    float y0 = x00 + v * (x10 - x00);
    float y1 = x01 + v * (x11 - x01);

    return y0 + w * (y1 - y0);
}

// 3D Worley (cellular) noise for billowy cauliflower cloud structures
float WorleyNoise3D(float x, float y, float z) {
    int ix = static_cast<int>(std::floor(x));
    int iy = static_cast<int>(std::floor(y));
    int iz = static_cast<int>(std::floor(z));

    float fx = x - static_cast<float>(ix);
    float fy = y - static_cast<float>(iy);
    float fz = z - static_cast<float>(iz);

    float minDistSq = 999.0f;

    for (int dz = -1; dz <= 1; ++dz) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                float cellHashX = Hash3D(ix + dx, iy + dy, iz + dz);
                float cellHashY = Hash3D(ix + dx + 13, iy + dy + 29, iz + dz + 47);
                float cellHashZ = Hash3D(ix + dx + 71, iy + dy + 83, iz + dz + 101);

                float cx = static_cast<float>(dx) + cellHashX;
                float cy = static_cast<float>(dy) + cellHashY;
                float cz = static_cast<float>(dz) + cellHashZ;

                float distSq = (fx - cx) * (fx - cx) + (fy - cy) * (fy - cy) + (fz - cz) * (fz - cz);
                if (distSq < minDistSq) {
                    minDistSq = distSq;
                }
            }
        }
    }

    return std::clamp(1.0f - std::sqrt(minDistSq), 0.0f, 1.0f);
}
} // namespace

float CloudGen::GetHeightFraction(const Vec3& worldPos) const {
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
            grad = CUMULUS_GRADIENT;
            break;
        case CloudType::Cumulonimbus:
            grad = CUMULONIMBUS_GRADIENT;
            break;
        case CloudType::Altocumulus:
            grad = ALTOCUMULUS_GRADIENT;
            break;
        case CloudType::Cirrus:
        default:
            grad = CIRRUS_GRADIENT;
            break;
    }

    // Meteoros smoothstep height gradient curve
    float bottomFade = Smoothstep(grad[0], grad[1], heightFraction);
    float topFade = 1.0f - Smoothstep(grad[2], grad[3], heightFraction);
    return std::max(0.0f, bottomFade * topFade);
}

float CloudGen::Beer(float opticalDepth) const {
    return std::exp(-opticalDepth);
}

float CloudGen::MultiBeer(float opticalDepth, float multiScatteringFactor) const {
    // Meteoros Dual Beer-Lambert multi-scattering approximation
    float primary = std::exp(-opticalDepth);
    float secondary = std::exp(-opticalDepth * 0.25f) * 0.7f;
    return primary + secondary * multiScatteringFactor;
}

float CloudGen::Powder(float opticalDepth) const {
    // Meteoros powder sugar effect: highlights outer edges, simulates multiple in-scattering
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
    return forward * 0.72f + backward * 0.28f;
}

float CloudGen::SampleCloudDensity(const Vec3& p, float time, bool expensive) const {
    float heightFraction = GetHeightFraction(p);
    if (heightFraction <= 0.005f || heightFraction >= 0.995f) {
        return 0.0f;
    }

    float typeDensity = GetDensityForCloud(heightFraction, m_params.type);
    if (typeDensity <= 0.005f) {
        return 0.0f;
    }

    // Wind advection animation & height shear offset (Meteoros wind model)
    Vec3 wind = m_params.windDirection;
    float shear = heightFraction * m_params.cloudTopOffset;
    float drift = time * (m_params.windSpeed * (1000.0f / 3600.0f) * 0.25f);

    // Spatial coordinate scale (calibrated for 2-8km natural cloud formations)
    float baseScale = 0.00022f * m_params.cloudScale;
    float px = (p.x + wind.x * (shear + drift)) * baseScale;
    float py = heightFraction * 3.0f;
    float pz = (p.z + wind.z * (shear + drift)) * baseScale;

    // 1. 3D Perlin-Worley FBM Macro Shape
    float p1 = ValueNoise3D(px * 1.5f, py * 1.2f, pz * 1.5f);
    float p2 = ValueNoise3D(px * 3.2f + 1.7f, py * 2.4f, pz * 3.2f + 2.3f) * 0.5f;
    float perlinFBM = (p1 + p2) * 0.666f;

    float w1 = WorleyNoise3D(px * 1.8f, py * 1.5f, pz * 1.8f);
    float w2 = WorleyNoise3D(px * 3.6f + 2.1f, py * 3.0f, pz * 3.6f + 1.9f) * 0.5f;
    float worleyFBM = (w1 + w2) * 0.666f;

    // Nubis / Decima Perlin-Worley combination: Perlin base with Worley billows
    float perlinWorley = perlinFBM * 0.55f + worleyFBM * 0.45f;

    // 2. Weather map coverage & thresholding
    float coverage = std::clamp(m_params.coverage, 0.05f, 0.98f);
    float threshold = 1.0f - coverage * 0.82f;
    if (perlinWorley < threshold - 0.12f) {
        return 0.0f;
    }

    float baseCloud = Remap(perlinWorley, threshold, 1.0f, 0.0f, 1.0f);
    baseCloud = std::clamp(baseCloud, 0.0f, 1.0f);
    baseCloud *= typeDensity;

    // 3. 3D Worley detail erosion along cloud boundaries (subtle to prevent grain)
    if (expensive && m_params.detailErosion > 0.01f && baseCloud > 0.01f) {
        float ex = (p.x - wind.x * drift * 0.5f) * baseScale * 2.2f;
        float ey = heightFraction * 6.0f;
        float ez = (p.z - wind.z * drift * 0.5f) * baseScale * 2.2f;

        float hfWorley = WorleyNoise3D(ex, ey, ez);
        float erosionFactor = (1.0f - hfWorley) * m_params.detailErosion * 0.18f;
        baseCloud = std::clamp(baseCloud - erosionFactor, 0.0f, 1.0f);
    }

    return std::clamp(baseCloud * m_params.density * 1.5f, 0.0f, 1.0f);
}

} // namespace sphere::weather
