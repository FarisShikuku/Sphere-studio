#pragma once
// ============================================================================
// ForestGrassGenerator.h
// Native C++17 Procedural Forest Grass & Understory Ground Flora Engine
// Part of the Unified Vegetation Subsystem (native-assets/src/vegetation/grass/)
//
// Conforms to woodland understory botany (Poa nemoralis, Brachypodium sylvaticum):
//  - Shade-tolerant blade morphology (increased surface area, thin lamina)
//  - De Casteljau quadratic Bezier spline curvature with moisture droop
//  - Canopy sunfleck and occlusion adaptation
//  - Clumping around tree root flares and forest floor humus patches
//  - Canopy-attenuated aerodynamic wind wave response
// ============================================================================

#include <vector>
#include <cstdint>
#include <cmath>
#include "GrassTypes.h"
#include "../common/VegetationMath.h"

namespace vegetation {

struct ForestGrassParams {
    float bladeDensityPerM2{650.0f};      // 200 - 1500 blades / m^2
    float meanBladeHeightMeters{0.32f};   // 0.15 - 0.55 m
    float meanBladeWidthMeters{0.024f};   // Wider blades for woodland shade capture
    float canopyOcclusion{0.72f};         // 0.0 (full sun) to 1.0 (dense canopy)
    float moistureContent{0.75f};         // 0.0 - 1.0 (influences droop and moss)
    float clumpSpreadMeters{1.8f};        // Radial size of understory grass clumps
    float windSpeedMps{3.5f};             // Ambient wind speed
    float canopyWindDamping{0.65f};       // Wind energy absorbed by forest canopy
    uint32_t randomSeed{2024};
};

struct ForestGrassBladeVertex {
    float px, py, pz;
    float nx, ny, nz;
    float u, v;
    float shadeOcclusion; // 0.0 (dark forest shade) to 1.0 (sunfleck)
};

struct ForestGrassMesh3D {
    std::vector<ForestGrassBladeVertex> vertices;
    std::vector<uint32_t> indices;
    size_t bladeCount{0};
};

struct ForestGrassPatch {
    float centerX;
    float centerZ;
    float radius;
    float densityFactor;
    float mossFraction;
};

class ForestGrassGenerator {
public:
    // Generate full forest grass mesh across a woodland terrain patch
    static ForestGrassMesh3D generateUnderstoryMesh(
        const ForestGrassParams& params,
        float areaWidthMeters = 20.0f,
        float areaDepthMeters = 20.0f,
        int segmentsPerBlade = 4
    );

    // Generate procedural forest grass clumps clustered near tree roots
    static std::vector<ForestGrassPatch> generateRootClumpPatches(
        const std::vector<Vec3_C>& treePositions,
        float maxPatchRadius = 3.0f,
        uint32_t seed = 42
    );

    // Evaluate De Casteljau quadratic Bezier point at parameter t in [0, 1]
    static Vec3_C evaluateDeCasteljauBezier(
        const Vec3_C& p0,
        const Vec3_C& p1,
        const Vec3_C& p2,
        float t
    );

    // Compute shade-adapted chlorophyll pigmentation
    // Returns RGB color (darker, rich emerald in deep shade; golden-green in sunflecks)
    static void computeForestBladeColor(
        float shadeFactor,
        float heightFrac,
        float& outR, float& outG, float& outB
    );

    // Calculate effective wind sway under canopy
    static float computeEffectiveWindSway(
        float baseWindSpeed,
        float canopyDamping,
        float timeSec,
        float worldX, float worldZ
    );
};

} // namespace vegetation
