// ============================================================================
// RealTimeGrassEngine.cpp
// Unified Native C++17 Real-Time Grass Pipeline (Monolithic & Modular Modes)
// ============================================================================

#include "RealTimeGrassEngine.h"
#include "ForestGrassGenerator.h"
#include <algorithm>
#include <chrono>
#include <cmath>

namespace vegetation {

namespace {
constexpr float kPi = 3.14159265358979323846f;
constexpr float kTwoPi = 6.28318530717958647692f;

inline uint32_t packArgb(float r, float g, float b) {
    uint32_t ir = static_cast<uint32_t>(std::clamp(r, 0.0f, 1.0f) * 255.0f);
    uint32_t ig = static_cast<uint32_t>(std::clamp(g, 0.0f, 1.0f) * 255.0f);
    uint32_t ib = static_cast<uint32_t>(std::clamp(b, 0.0f, 1.0f) * 255.0f);
    return (0xFFU << 24) | (ir << 16) | (ig << 8) | ib;
}

struct GrassSpeciesStyle {
    float rootR, rootG, rootB;
    float midR, midG, midB;
    float tipR, tipG, tipB;
    float tussockFactor;
};

constexpr GrassSpeciesStyle kGrassSpeciesStyles[12] = {
    // 0: Perennial Ryegrass (Lolium perenne) - emerald turf & meadows
    {0.07f, 0.24f, 0.07f,  0.18f, 0.56f, 0.16f,  0.38f, 0.80f, 0.28f,  0.00f},
    // 1: Woodland Brome (Bromus ramosus) - mossy forest understory shade
    {0.05f, 0.18f, 0.08f,  0.14f, 0.44f, 0.18f,  0.34f, 0.68f, 0.30f,  0.22f},
    // 2: Kentucky Bluegrass (Poa pratensis) - velvet blue-jade carpet
    {0.05f, 0.20f, 0.15f,  0.12f, 0.48f, 0.36f,  0.28f, 0.72f, 0.54f,  0.00f},
    // 3: Savanna Red Oat (Themeda triandra) - olive-ochre to golden-copper savanna
    {0.16f, 0.22f, 0.07f,  0.52f, 0.48f, 0.16f,  0.84f, 0.42f, 0.18f,  0.62f},
    // 4: Highland Kikuyu (Pennisetum clandestinum) - lush vibrant chartreuse-emerald
    {0.08f, 0.28f, 0.06f,  0.26f, 0.66f, 0.14f,  0.54f, 0.88f, 0.24f,  0.08f},
    // 5: Elephant Napier (Cenchrus purpureus) - tall cane-green tropical clumps
    {0.12f, 0.24f, 0.09f,  0.24f, 0.54f, 0.18f,  0.48f, 0.76f, 0.28f,  0.70f},
    // 6: Coastal Marram (Ammophila arenaria) - glaucous silver-sage dunes
    {0.18f, 0.26f, 0.18f,  0.42f, 0.58f, 0.44f,  0.76f, 0.78f, 0.56f,  0.78f},
    // 7: Blue Fescue (Festuca glauca) - icy powder-blue alpine hedgehog tussocks
    {0.10f, 0.22f, 0.30f,  0.28f, 0.56f, 0.68f,  0.56f, 0.82f, 0.90f,  0.85f},
    // 8: Purple Fountain (Cenchrus setaceus) - burgundy-plum to rose-magenta plumes
    {0.16f, 0.05f, 0.12f,  0.46f, 0.14f, 0.28f,  0.78f, 0.34f, 0.50f,  0.80f},
    // 9: Pampas Plume (Cortaderia selloana) - sage stems with creamy-silver plumes
    {0.20f, 0.24f, 0.12f,  0.48f, 0.52f, 0.28f,  0.90f, 0.88f, 0.76f,  0.84f},
    // 10: Japanese Blood Grass (Imperata cylindrica 'Rubra') - lime base flaring scarlet
    {0.12f, 0.30f, 0.08f,  0.56f, 0.24f, 0.12f,  0.88f, 0.18f, 0.16f,  0.40f},
    // 11: Prairie Bluestem (Andropogon gerardii) - slate-blue flushing violet-amber tips
    {0.12f, 0.24f, 0.24f,  0.38f, 0.38f, 0.46f,  0.74f, 0.44f, 0.26f,  0.72f}
};
} // namespace

void RealTimeGrassEngine::sampleForceMapVector(
    GrassForceStamp stamp,
    float u,
    float v,
    float& outFx,
    float& outFy,
    float& outFz,
    float& outMagnitude
) {
    float cx = u - 0.5f;
    float cz = v - 0.5f;
    outFx = 0.0f;
    outFy = 0.0f;
    outFz = 0.0f;
    outMagnitude = 0.0f;

    switch (stamp) {
        case GrassForceStamp::TRIFORCE_EMBLEM: {
            bool inTop = (v > 0.50f && v < 0.84f && std::abs(cx) < (0.84f - v) * 0.68f);
            bool inBotL = (v > 0.18f && v <= 0.50f && std::abs(cx + 0.17f) < (0.50f - v) * 0.68f);
            bool inBotR = (v > 0.18f && v <= 0.50f && std::abs(cx - 0.17f) < (0.50f - v) * 0.68f);
            if (inTop || inBotL || inBotR) {
                float dirLen = std::sqrt(cx * cx + cz * cz) + 1e-4f;
                outFx = (cx / dirLen) * 0.95f;
                outFy = -0.88f;
                outFz = (cz / dirLen) * 0.95f;
                outMagnitude = 1.0f;
            }
            break;
        }
        case GrassForceStamp::RADIAL_VORTEX: {
            float r = std::sqrt(cx * cx + cz * cz);
            if (r < 0.40f && r > 1e-4f) {
                float w = (0.40f - r) / 0.40f;
                float rx = cx / r;
                float rz = cz / r;
                // Swirling tangential + radial outward vortex force
                outFx = (rx * 0.45f - rz * 0.85f) * w;
                outFy = -0.78f * w;
                outFz = (rz * 0.45f + rx * 0.85f) * w;
                outMagnitude = w;
            }
            break;
        }
        case GrassForceStamp::CORRIDOR_TRAIL: {
            float pathX = 0.18f * std::sin(v * kTwoPi);
            float dist = std::abs(cx - pathX);
            if (dist < 0.16f) {
                float push = (cx >= pathX) ? 1.0f : -1.0f;
                float w = (0.16f - dist) / 0.16f;
                outFx = push * 0.95f * w;
                outFy = -0.82f * w;
                outFz = 0.22f * w;
                outMagnitude = w;
            }
            break;
        }
        case GrassForceStamp::NONE_NATURAL:
        default:
            break;
    }
}

void RealTimeGrassEngine::evaluateField(
    const RealTimeGrassConfig& cfg,
    std::vector<RealTimeGrassBladeOut>& outBlades,
    float* outForceGrid24x24,
    RealTimeGrassStats& outStats
) {
    auto tStart = std::chrono::steady_clock::now();

    const int count = std::clamp(cfg.bladesPerPatch, 40, 900);
    const float halfPatch = std::max(0.5f, cfg.patchSizeMeters * 0.5f);

    // Camera position matching viewport orbit coordinates
    const float camAzRad = cfg.camAzimuthDeg * (kPi / 180.0f);
    const float camElRad = std::clamp(cfg.camElevationDeg, 10.0f, 82.0f) * (kPi / 180.0f);
    const float camDistOrbit = std::max(1.5f, cfg.camDistanceMeters);
    const float camX = camDistOrbit * std::cos(camElRad) * std::sin(camAzRad);
    const float camY = 0.10f + camDistOrbit * std::sin(camElRad);
    const float camZ = camDistOrbit * std::cos(camElRad) * std::cos(camAzRad);

    // 24x24 2D force map preview synthesis
    if (outForceGrid24x24) {
        const int res = 24;
        for (int r = 0; r < res; ++r) {
            float v = static_cast<float>(r) / static_cast<float>(res - 1);
            for (int c = 0; c < res; ++c) {
                float u = static_cast<float>(c) / static_cast<float>(res - 1);
                float fx, fy, fz, mag;
                if (cfg.forceStamp == GrassForceStamp::NONE_NATURAL) {
                    float wx = (u - 0.5f) * 2.4f;
                    float wave = 0.5f + 0.5f * std::sin(0.65f * kPi * wx + cfg.timeSeconds * cfg.windSpeed);
                    mag = wave * 0.65f;
                } else {
                    sampleForceMapVector(cfg.forceStamp, u, v, fx, fy, fz, mag);
                    if (mag <= 0.01f) mag = 0.10f;
                }
                outForceGrid24x24[r * res + c] = std::clamp(mag * std::max(0.25f, cfg.forceMapStrength), 0.0f, 1.2f);
            }
        }
    }

    outBlades.clear();
    outBlades.reserve(static_cast<size_t>(count));

    // Sun direction vector for two-sided optical blade shading
    float sunX = 0.42f, sunY = 0.84f, sunZ = 0.34f;
    float sunLen = std::sqrt(sunX * sunX + sunY * sunY + sunZ * sunZ);
    sunX /= sunLen; sunY /= sunLen; sunZ /= sunLen;

    // Stratified golden-ratio + LCG deterministic distribution for uniform lush coverage without gaps
    uint64_t rng = (cfg.mode == GrassPipelineMode::MONOLITHIC)
        ? 0xCAFEBABE12345678ULL
        : 0x5F3759DF98765432ULL;

    auto nextRand = [&rng]() -> float {
        rng = rng * 6364136223846793005ULL + 1442695040888963407ULL;
        return static_cast<float>((rng >> 32) & 0x7FFFFFFFULL) / static_cast<float>(0x7FFFFFFFULL);
    };

    const int gridSide = static_cast<int>(std::ceil(std::sqrt(static_cast<float>(count))));
    const float cellSpan = cfg.patchSizeMeters / static_cast<float>(std::max(1, gridSide));

    int activeCount = 0;
    int culledCount = 0;
    double tessSum = 0.0;

    const int speciesIdx = std::clamp(cfg.speciesId, 0, 11);
    const GrassSpeciesStyle& spStyle = kGrassSpeciesStyles[speciesIdx];

    // Precompute local tussock hubs for tufted/bunchgrass species
    constexpr int kNumClumps = 16;
    float clumpX[kNumClumps];
    float clumpZ[kNumClumps];
    for (int c = 0; c < kNumClumps; ++c) {
        int cgx = c % 4;
        int cgz = c / 4;
        float cx = -halfPatch + (static_cast<float>(cgx) + 0.24f + 0.52f * nextRand()) * (cfg.patchSizeMeters * 0.25f);
        float cz = -halfPatch + (static_cast<float>(cgz) + 0.24f + 0.52f * nextRand()) * (cfg.patchSizeMeters * 0.25f);
        clumpX[c] = cx;
        clumpZ[c] = cz;
    }

    const float maxDist = std::max(3.0f, cfg.maxCullDistance);

    for (int i = 0; i < count; ++i) {
        float r0 = nextRand();
        float r1 = nextRand();
        float r2 = nextRand();
        float r3 = nextRand();
        float r4 = nextRand();
        float r5 = nextRand();
        float r6 = nextRand();
        float r7 = nextRand();
        float rJx = nextRand();
        float rJz = nextRand();
        float rW = nextRand();
        float rH = nextRand();

        int gx = i % gridSide;
        int gz = i / gridSide;

        // Stratified cell jitter for smooth, natural meadow distribution
        float rootX = -halfPatch + (static_cast<float>(gx) + 0.15f + 0.70f * rJx) * cellSpan;
        float rootZ = -halfPatch + (static_cast<float>(gz) + 0.15f + 0.70f * rJz) * cellSpan;

        // Pull tufted/tussock species roots smoothly toward local clump hubs
        float radOutX = 0.0f;
        float radOutZ = 0.0f;
        if (spStyle.tussockFactor > 0.01f) {
            int nearest = 0;
            float bestD2 = 1e9f;
            for (int c = 0; c < kNumClumps; ++c) {
                float ddx = rootX - clumpX[c];
                float ddz = rootZ - clumpZ[c];
                float d2 = ddx * ddx + ddz * ddz;
                if (d2 < bestD2) {
                    bestD2 = d2;
                    nearest = c;
                }
            }
            float pull = spStyle.tussockFactor * (0.68f + 0.24f * r4);
            rootX = rootX + (clumpX[nearest] - rootX) * pull;
            rootZ = rootZ + (clumpZ[nearest] - rootZ) * pull;
            float odx = rootX - clumpX[nearest];
            float odz = rootZ - clumpZ[nearest];
            float oLen = std::sqrt(odx * odx + odz * odz);
            if (oLen > 1e-4f) {
                radOutX = odx / oLen;
                radOutZ = odz / oLen;
            }
        }

        rootX = std::clamp(rootX, -halfPatch + 0.02f, halfPatch - 0.02f);
        rootZ = std::clamp(rootZ, -halfPatch + 0.02f, halfPatch - 0.02f);
        float rootY = 0.0f;

        // Distance from camera for TCS tessellation LOD and culling
        float dxC = rootX - camX;
        float dyC = rootY - camY;
        float dzC = rootZ - camZ;
        float camDist = std::sqrt(dxC * dxC + dyC * dyC + dzC * dzC);

        float distRatio = camDist / maxDist;
        if (distRatio >= 1.0f) {
            culledCount++;
            continue;
        }

        int tessLevel = static_cast<int>(std::ceil(cfg.maxTessellationLevel * (1.0f - distRatio)));
        tessLevel = std::clamp(tessLevel, 1, 8);

        float width = cfg.minBladeWidth + rW * std::max(0.002f, cfg.maxBladeWidth - cfg.minBladeWidth);
        float height = cfg.minBladeHeight + rH * std::max(0.02f, cfg.maxBladeHeight - cfg.minBladeHeight);
        float orientationRad = kTwoPi * r0;

        // Resting botanical arch perpendicular to blade width (and radial splay for tussock species)
        float archDirX = -std::sin(orientationRad);
        float archDirZ = std::cos(orientationRad);
        if (spStyle.tussockFactor > 0.25f) {
            float mixR = spStyle.tussockFactor * 0.65f;
            archDirX = archDirX * (1.0f - mixR) + radOutX * mixR;
            archDirZ = archDirZ * (1.0f - mixR) + radOutZ * mixR;
        }
        float staticArch = cfg.maxBending * (0.9f + 1.4f * r2);
        float dispX = archDirX * staticArch + cfg.maxBending * (2.0f * r3 - 1.0f) * 0.35f;
        float dispY = -staticArch * 0.22f;
        float dispZ = archDirZ * staticArch + cfg.maxBending * (2.0f * r7 - 1.0f) * 0.35f;

        // Aerodynamic Wind Displacement
        if (cfg.windActive) {
            if (cfg.mode == GrassPipelineMode::MONOLITHIC) {
                // Jahrmann & Wimmer (2013) directional traveling sine wave
                float phase = 0.65f * kPi * rootX + 0.35f * rootZ + cfg.timeSeconds * cfg.windSpeed;
                float gust = (std::sin(phase) * 0.72f + std::sin(phase * 2.13f + 0.7f) * 0.28f) * cfg.windStrength;
                float leanMag = (height / 0.32f);
                dispX += gust * 0.085f * leanMag;
                dispZ += gust * 0.115f * leanMag;
                dispY -= std::abs(gust) * 0.032f * leanMag;
            } else {
                // Modular 4-stage pipeline: traveling wave + cross-wind canopy micro-flutter
                float sway = ForestGrassGenerator::computeEffectiveWindSway(
                    cfg.windSpeed * cfg.windStrength,
                    0.35f,
                    cfg.timeSeconds,
                    rootX,
                    rootZ
                );
                float flutter = std::sin(cfg.timeSeconds * (cfg.windSpeed * 1.4f) + rootX * 2.4f - rootZ * 1.9f) * 0.045f * cfg.windStrength;
                float leanMag = (height / 0.32f);
                dispX += (sway * 0.75f + flutter) * leanMag;
                dispZ += (sway * 0.95f + flutter * 0.6f) * leanMag;
                dispY -= (std::abs(sway) * 0.14f) * leanMag;
            }
        }

        // 2D Force Map Sampling (u, v in [0, 1])
        float mapU = std::clamp((rootX + halfPatch) / cfg.patchSizeMeters, 0.0f, 1.0f);
        float mapV = std::clamp((rootZ + halfPatch) / cfg.patchSizeMeters, 0.0f, 1.0f);
        float fx = 0.0f, fy = 0.0f, fz = 0.0f, fMag = 0.0f;
        sampleForceMapVector(cfg.forceStamp, mapU, mapV, fx, fy, fz, fMag);

        bool isDepressed = (fMag * cfg.forceMapStrength) > 0.24f;
        float forceScale = cfg.forceMapStrength;
        dispX += fx * 0.28f * forceScale;
        dispY += fy * (height * 0.62f) * forceScale;
        dispZ += fz * 0.28f * forceScale;

        float tipX = rootX + dispX;
        float tipY = std::max(0.025f, rootY + height + dispY);
        float tipZ = rootZ + dispZ;

        // De Casteljau quadratic Bezier control point P1
        // Place control point mid-way upright so the blade curves smoothly outward rather than kinking
        float ctrlFlex = 0.22f + r1 * 0.16f;
        float ctrlH = 0.55f + r7 * 0.14f;
        float ctrlX = rootX + (tipX - rootX) * ctrlFlex;
        float ctrlY = rootY + (tipY - rootY) * ctrlH;
        float ctrlZ = rootZ + (tipZ - rootZ) * ctrlFlex;

        // Evaluate spline midpoint and tangent (De Casteljau at v = 0.55)
        Vec3_C p0{rootX, rootY, rootZ};
        Vec3_C p1{ctrlX, ctrlY, ctrlZ};
        Vec3_C p2{tipX, tipY, tipZ};
        float vParam = 0.55f;
        float invV = 1.0f - vParam;
        float ax = invV * p0.x + vParam * p1.x;
        float ay = invV * p0.y + vParam * p1.y;
        float az = invV * p0.z + vParam * p1.z;
        float bx = invV * p1.x + vParam * p2.x;
        float by = invV * p1.y + vParam * p2.y;
        float bz = invV * p1.z + vParam * p2.z;

        float tanX = bx - ax;
        float tanY = by - ay;
        float tanZ = bz - az;
        float tanLen = std::sqrt(tanX * tanX + tanY * tanY + tanZ * tanZ);
        if (tanLen > 1e-5f) {
            tanX /= tanLen; tanY /= tanLen; tanZ /= tanLen;
        } else {
            tanX = 0.0f; tanY = 1.0f; tanZ = 0.0f;
        }

        float bitanX = std::cos(orientationRad);
        float bitanY = 0.0f;
        float bitanZ = std::sin(orientationRad);

        float nx = tanY * bitanZ - tanZ * bitanY;
        float ny = tanZ * bitanX - tanX * bitanZ;
        float nz = tanX * bitanY - tanY * bitanX;
        float nLen = std::sqrt(nx * nx + ny * ny + nz * nz);
        if (nLen > 1e-5f) {
            nx /= nLen; ny /= nLen; nz /= nLen;
        } else {
            nx = 0.0f; ny = 1.0f; nz = 0.0f;
        }

        // Two-sided diffuse lighting + sunfleck / AO root-to-tip species gradient
        float nDotL = std::abs(nx * sunX + ny * sunY + nz * sunZ);
        float shadeBase = 0.56f + 0.44f * nDotL;

        float sunfleck = 0.5f + 0.5f * std::sin(rootX * 1.4f + 0.5f) * std::cos(rootZ * 1.4f - 0.3f);
        float varR = 1.0f + (r4 * 0.24f - 0.12f);
        float varG = 1.0f + (r5 * 0.20f - 0.10f);
        float varB = 1.0f + (r6 * 0.20f - 0.10f);
        float fleckGain = (cfg.mode == GrassPipelineMode::MODULAR) ? (0.92f + 0.16f * sunfleck) : 1.0f;

        // Subtle warm golden-olive tint when strongly bent/parted by 2D force map
        float forceTint = isDepressed ? 0.10f : 0.0f;

        uint32_t rootArgb = packArgb(
            spStyle.rootR * varR + forceTint * 0.35f,
            spStyle.rootG * varG + forceTint * 0.20f,
            spStyle.rootB * varB
        );
        uint32_t midArgb = packArgb(
            (spStyle.midR * varR * fleckGain + forceTint * 0.85f) * shadeBase,
            (spStyle.midG * varG * fleckGain + forceTint * 0.40f) * shadeBase,
            (spStyle.midB * varB * fleckGain) * shadeBase
        );
        uint32_t tipArgb = packArgb(
            (spStyle.tipR * varR * fleckGain + forceTint * 0.90f) * (0.72f + 0.34f * nDotL),
            (spStyle.tipG * varG * fleckGain + forceTint * 0.45f) * (0.74f + 0.34f * nDotL),
            (spStyle.tipB * varB * fleckGain) * (0.72f + 0.30f * nDotL)
        );

        outBlades.push_back({
            rootX, rootY, rootZ,
            ctrlX, ctrlY, ctrlZ,
            tipX, tipY, tipZ,
            nx, ny, nz,
            orientationRad,
            width,
            tessLevel,
            isDepressed,
            rootArgb,
            midArgb,
            tipArgb
        });

        activeCount++;
        tessSum += static_cast<double>(tessLevel);
    }

    auto tEnd = std::chrono::steady_clock::now();
    float execMs = std::chrono::duration<float, std::milli>(tEnd - tStart).count();

    outStats.activeBladesCount = activeCount;
    outStats.culledBladesCount = culledCount;
    outStats.averageTessLevel = (activeCount > 0) ? static_cast<float>(tessSum / static_cast<double>(activeCount)) : 0.0f;
    outStats.executionTimeMs = std::max(0.01f, execMs);
}

} // namespace vegetation
