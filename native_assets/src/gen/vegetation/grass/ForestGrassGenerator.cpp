// ============================================================================
// ForestGrassGenerator.cpp
// Native C++17 Procedural Forest Grass & Understory Flora Implementation
// Part of the Unified Vegetation Subsystem (native-assets/src/vegetation/grass/)
// ============================================================================

#include "ForestGrassGenerator.h"
#include "../../core/Assert.h"
#include <cmath>
#include <algorithm>

namespace vegetation {

namespace {
constexpr float PI_F = 3.14159265358979323846f;
constexpr float TWO_PI_F = 6.28318530717958647692f;
}

Vec3_C ForestGrassGenerator::evaluateDeCasteljauBezier(
    const Vec3_C& p0,
    const Vec3_C& p1,
    const Vec3_C& p2,
    float t
) {
    // Linear interpolation step 1
    float invT = 1.0f - t;
    float q0x = invT * p0.x + t * p1.x;
    float q0y = invT * p0.y + t * p1.y;
    float q0z = invT * p0.z + t * p1.z;

    float q1x = invT * p1.x + t * p2.x;
    float q1y = invT * p1.y + t * p2.y;
    float q1z = invT * p1.z + t * p2.z;

    // Linear interpolation step 2 (final Bezier point)
    return Vec3_C{
        invT * q0x + t * q1x,
        invT * q0y + t * q1y,
        invT * q0z + t * q1z
    };
}

void ForestGrassGenerator::computeForestBladeColor(
    float shadeFactor,
    float heightFrac,
    float& outR, float& outG, float& outB
) {
    // Deep forest understory has richer emerald/forest green (more chlorophyll-b)
    // Sunflecks lighten the blade to a vibrant olive/chartreuse
    float baseR = 0.12f * shadeFactor + 0.05f * (1.0f - shadeFactor);
    float baseG = 0.38f * shadeFactor + 0.22f * (1.0f - shadeFactor);
    float baseB = 0.09f * shadeFactor + 0.04f * (1.0f - shadeFactor);

    // Tip gradient (younger cells at tip are slightly brighter)
    float tipLighten = 0.15f * heightFrac;
    outR = std::clamp(baseR + tipLighten * 0.4f, 0.0f, 1.0f);
    outG = std::clamp(baseG + tipLighten, 0.0f, 1.0f);
    outB = std::clamp(baseB + tipLighten * 0.2f, 0.0f, 1.0f);
}

float ForestGrassGenerator::computeEffectiveWindSway(
    float baseWindSpeed,
    float canopyDamping,
    float timeSec,
    float worldX, float worldZ
) {
    // Under canopy, wind is attenuated by canopyDamping fraction
    float effectiveWind = baseWindSpeed * (1.0f - std::clamp(canopyDamping, 0.0f, 0.90f));

    // Slow travelling wind gusts through understory
    float wavePhase = timeSec * 1.8f + worldX * 0.25f + worldZ * 0.20f;
    float microTurbulence = std::sin(timeSec * 3.5f + worldX * 1.1f) * 0.25f;

    float gust = std::sin(wavePhase) + microTurbulence;
    return effectiveWind * 0.08f * gust;
}

std::vector<ForestGrassPatch> ForestGrassGenerator::generateRootClumpPatches(
    const std::vector<Vec3_C>& treePositions,
    float maxPatchRadius,
    uint32_t seed
) {
    std::vector<ForestGrassPatch> patches;
    patches.reserve(treePositions.size() * 3);

    uint32_t rng = seed;
    auto lcg = [&rng]() -> float {
        rng = rng * 1664525u + 1013904223u;
        return static_cast<float>(rng & 0x00FFFFFFu) / static_cast<float>(0x01000000u);
    };

    for (const auto& treePos : treePositions) {
        // Clumps form in drip line ring around trunk (1.5m to 3.5m radius)
        int clumpCount = 2 + static_cast<int>(lcg() * 3.0f);
        for (int c = 0; c < clumpCount; ++c) {
            float angle = lcg() * TWO_PI_F;
            float dist = 1.2f + lcg() * (maxPatchRadius - 1.2f);

            ForestGrassPatch patch;
            patch.centerX = treePos.x + dist * std::cos(angle);
            patch.centerZ = treePos.z + dist * std::sin(angle);
            patch.radius = 0.8f + lcg() * 1.2f;
            patch.densityFactor = 0.8f + lcg() * 0.5f;
            // High moisture near north-facing root hollows -> moss accumulation
            patch.mossFraction = 0.35f + lcg() * 0.45f;
            patches.push_back(patch);
        }
    }

    return patches;
}

ForestGrassMesh3D ForestGrassGenerator::generateUnderstoryMesh(
    const ForestGrassParams& params,
    float areaWidthMeters,
    float areaDepthMeters,
    int segmentsPerBlade
) {
    ENGINE_CORE_ASSERT(segmentsPerBlade >= 2, "segmentsPerBlade must be >= 2");
    ENGINE_CORE_ASSERT(areaWidthMeters > 0.0f, "areaWidthMeters must be > 0");
    ENGINE_CORE_ASSERT(areaDepthMeters > 0.0f, "areaDepthMeters must be > 0");

    ForestGrassMesh3D mesh;
    uint32_t rng = params.randomSeed;
    auto lcg = [&rng]() -> float {
        rng = rng * 1664525u + 1013904223u;
        return static_cast<float>(rng & 0x00FFFFFFu) / static_cast<float>(0x01000000u);
    };

    // Calculate approximate blade count based on density
    float totalArea = areaWidthMeters * areaDepthMeters;
    // Modulate by canopy occlusion (denser in dappled light, sparser in total shade)
    float effectiveDensity = params.bladeDensityPerM2 * (1.2f - params.canopyOcclusion * 0.4f);
    size_t targetBlades = static_cast<size_t>(std::clamp(totalArea * effectiveDensity * 0.015f, 40.0f, 1200.0f));

    mesh.bladeCount = targetBlades;
    mesh.vertices.reserve(targetBlades * (segmentsPerBlade + 1) * 2);
    mesh.indices.reserve(targetBlades * segmentsPerBlade * 6);

    const float halfW = areaWidthMeters * 0.5f;
    const float halfD = areaDepthMeters * 0.5f;

    for (size_t b = 0; b < targetBlades; ++b) {
        float rootX = -halfW + lcg() * areaWidthMeters;
        float rootZ = -halfD + lcg() * areaDepthMeters;
        float rootY = 0.0f;

        // Blade morphology with natural variation
        float height = params.meanBladeHeightMeters * (0.75f + lcg() * 0.50f);
        float width = params.meanBladeWidthMeters * (0.80f + lcg() * 0.40f);
        float heading = lcg() * TWO_PI_F;

        // Moisture droop: softer curve under higher moisture
        float droop = (0.25f + params.moistureContent * 0.20f) * (0.8f + lcg() * 0.4f);

        // De Casteljau control points
        Vec3_C p0{rootX, rootY, rootZ};
        Vec3_C p1{
            rootX + std::cos(heading) * (height * 0.35f),
            rootY + height * 0.60f,
            rootZ + std::sin(heading) * (height * 0.35f)
        };
        Vec3_C p2{
            rootX + std::cos(heading) * (height * (0.65f + droop)),
            rootY + height * (0.90f - droop * 0.3f),
            rootZ + std::sin(heading) * (height * (0.65f + droop))
        };

        // Lateral normal vector
        float perpX = -std::sin(heading);
        float perpZ = std::cos(heading);

        uint32_t baseVertexIdx = static_cast<uint32_t>(mesh.vertices.size());

        for (int s = 0; s <= segmentsPerBlade; ++s) {
            float t = static_cast<float>(s) / static_cast<float>(segmentsPerBlade);
            Vec3_C centerPt = evaluateDeCasteljauBezier(p0, p1, p2, t);

            // Blade width tapers from root to pointed tip
            float currentHalfW = (width * 0.5f) * (1.0f - t * 0.90f);

            // Shade factor: modulated across space (simulating canopy sunflecks)
            float sunfleckNoise = std::sin(rootX * 0.4f) * std::cos(rootZ * 0.4f);
            float shadeOcclusion = std::clamp(1.0f - params.canopyOcclusion + sunfleckNoise * 0.25f, 0.15f, 0.95f);

            // Left vertex
            ForestGrassBladeVertex vL;
            vL.px = centerPt.x - perpX * currentHalfW;
            vL.py = centerPt.y;
            vL.pz = centerPt.z - perpZ * currentHalfW;
            vL.nx = perpX;
            vL.ny = 0.2f;
            vL.nz = perpZ;
            vL.u = 0.0f;
            vL.v = t;
            vL.shadeOcclusion = shadeOcclusion;

            // Right vertex
            ForestGrassBladeVertex vR;
            vR.px = centerPt.x + perpX * currentHalfW;
            vR.py = centerPt.y;
            vR.pz = centerPt.z + perpZ * currentHalfW;
            vR.nx = perpX;
            vR.ny = 0.2f;
            vR.nz = perpZ;
            vR.u = 1.0f;
            vR.v = t;
            vR.shadeOcclusion = shadeOcclusion;

            mesh.vertices.push_back(vL);
            mesh.vertices.push_back(vR);
        }

        // Triangle strips for blade segments
        for (int s = 0; s < segmentsPerBlade; ++s) {
            uint32_t v0 = baseVertexIdx + s * 2;
            uint32_t v1 = baseVertexIdx + s * 2 + 1;
            uint32_t v2 = baseVertexIdx + (s + 1) * 2;
            uint32_t v3 = baseVertexIdx + (s + 1) * 2 + 1;

            mesh.indices.push_back(v0);
            mesh.indices.push_back(v2);
            mesh.indices.push_back(v1);

            mesh.indices.push_back(v1);
            mesh.indices.push_back(v2);
            mesh.indices.push_back(v3);
        }
    }

    return mesh;
}

} // namespace vegetation
