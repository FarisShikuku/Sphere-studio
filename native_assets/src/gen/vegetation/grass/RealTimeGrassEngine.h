#pragma once
// ============================================================================
// RealTimeGrassEngine.h
// Unified Native C++17 Real-Time Grass Pipeline (Monolithic & Modular Modes)
// Part of native-assets/src/vegetation/grass/
//
// Implements Jahrmann & Wimmer (2013) De Casteljau quadratic Bezier blades,
// TCS distance-adaptive tessellation LOD, sinusoidal & curl-wind fields,
// 2D Force Map deformation stamps, and two-sided Lambertian shading.
// ============================================================================

#include <cstdint>
#include <vector>

namespace vegetation {

enum class GrassPipelineMode : int {
    MONOLITHIC = 0,
    MODULAR = 1
};

enum class GrassForceStamp : int {
    TRIFORCE_EMBLEM = 0,
    RADIAL_VORTEX = 1,
    CORRIDOR_TRAIL = 2,
    NONE_NATURAL = 3
};

struct RealTimeGrassConfig {
    GrassPipelineMode mode{GrassPipelineMode::MODULAR};
    int speciesId{0};
    int bladesPerPatch{520};
    float patchSizeMeters{2.4f};
    bool windActive{true};
    float windSpeed{2.6f};
    float windStrength{1.25f};
    float maxBending{0.06f};
    float minBladeWidth{0.008f};
    float maxBladeWidth{0.016f};
    float minBladeHeight{0.18f};
    float maxBladeHeight{0.38f};
    float maxTessellationLevel{8.0f};
    float maxCullDistance{15.0f};
    GrassForceStamp forceStamp{GrassForceStamp::TRIFORCE_EMBLEM};
    float forceMapStrength{1.0f};
    float timeSeconds{0.0f};
    float camAzimuthDeg{28.0f};
    float camElevationDeg{28.0f};
    float camDistanceMeters{6.0f};
};

struct RealTimeGrassBladeOut {
    float rootX, rootY, rootZ;
    float ctrlX, ctrlY, ctrlZ;
    float tipX, tipY, tipZ;
    float normalX, normalY, normalZ;
    float orientationRad;
    float width;
    int tessellationLevel;
    bool isForceDepressed;
    uint32_t rootColorArgb;
    uint32_t midColorArgb;
    uint32_t tipColorArgb;
};

struct RealTimeGrassStats {
    int activeBladesCount{0};
    int culledBladesCount{0};
    float averageTessLevel{0.0f};
    float executionTimeMs{0.0f};
};

class RealTimeGrassEngine {
public:
    static void evaluateField(
        const RealTimeGrassConfig& cfg,
        std::vector<RealTimeGrassBladeOut>& outBlades,
        float* outForceGrid24x24,
        RealTimeGrassStats& outStats
    );

    static void sampleForceMapVector(
        GrassForceStamp stamp,
        float u,
        float v,
        float& outFx,
        float& outFy,
        float& outFz,
        float& outMagnitude
    );
};

} // namespace vegetation
