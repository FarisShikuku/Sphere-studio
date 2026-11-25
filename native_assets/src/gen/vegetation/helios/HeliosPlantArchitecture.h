#pragma once
// ============================================================================
// HeliosPlantArchitecture.h - Native C++17 Plant Geometry & Phytomer Architecture
// Based on PlantSimulationLab/Helios (UC Davis / Bailey et al.)
// https://github.com/PlantSimulationLab/Helios
//
// Concepts:
// - Primitive geometric objects: Cylinders (internodes), Patches (leaf blades), Disks (flowers/tubers)
// - Phytomer shoot modules: [Internode + Petiole + Blade + Axillary Bud]
// - Fibonacci Golden Angle phyllotaxis (137.507764 deg) & Decussate branching
// - Structural procedural synthesis for shrubs, crops, and desert succulent columns
// ============================================================================

#include "../common/VegetationMath.h"
#include <vector>
#include <string>
#include <cstdint>

namespace vegetation {
namespace helios {

enum class HeliosPrimitiveType : uint32_t {
    CYLINDER_INTERNODE = 0,
    PATCH_LEAF_BLADE = 1,
    DISK_FLOWER = 2,
    RIBBED_PLEAT = 3
};

struct HeliosVertex {
    Vec3 position;
    Vec3 normal;
    Vec2 uv;
    Vec3 color;
    float absorbedPAR{0.0f}; // Radiation interception calculated by HeliosCanopyEngine
};

struct HeliosPhytomer {
    uint32_t phytomerId{0};
    uint32_t parentPhytomerId{0};
    uint32_t order{0}; // 0 = main stem/trunk, 1 = primary lateral, 2 = secondary
    Vec3 basePosition;
    Vec3 tipPosition;
    float internodeLengthMeters{0.12f};
    float internodeRadiusMeters{0.015f};
    float petioleLengthMeters{0.04f};
    float leafAreaSquareMeters{0.0035f};
    float leafPitchDeg{35.0f};
    float leafRollDeg{10.0f};
    float phyllotaxisAngleDeg{137.508f};
    Vec3 stemColor{0.35f, 0.25f, 0.18f};
    Vec3 leafColor{0.18f, 0.48f, 0.20f};
    bool hasFlower{false};
    Vec3 flowerColor{0.95f, 0.82f, 0.25f};
};

struct HeliosCanopyMesh {
    std::vector<HeliosVertex> vertices;
    std::vector<uint32_t> indices;
    Vec3 aabbMin{1e9f, 1e9f, 1e9f};
    Vec3 aabbMax{-1e9f, -1e9f, -1e9f};
    uint32_t totalPhytomerCount{0};
    float totalLeafAreaM2{0.0f};
    std::string architectureName;
};

/**
 * HeliosPlantArchitecture - Procedural Generator for Plant Architectures
 */
class HeliosPlantArchitecture {
public:
    HeliosPlantArchitecture() = default;
    ~HeliosPlantArchitecture() = default;

    // Generates a multi-shoot shrub architecture based on Helios phytomer assembly
    static HeliosCanopyMesh generateHeliosShrub(
        float heightMeters,
        float spreadMeters,
        int primaryCanes,
        int phytomersPerCane,
        float leafScale,
        uint64_t seed = 42
    );

    // Generates an agricultural crop furrow row based on Helios orchard/row crop module
    static HeliosCanopyMesh generateHeliosCropRow(
        float rowLengthMeters,
        float plantSpacingMeters,
        float cropHeightMeters,
        uint64_t seed = 101
    );

    // Generates a xerophytic cactus architecture with Helios radiation calculation
    static HeliosCanopyMesh generateHeliosDesertSucculent(
        int speciesArchetypeIndex,
        float heightMeters,
        float spreadMeters,
        int ribCount,
        uint64_t seed = 777
    );

private:
    static void appendCylinder(
        HeliosCanopyMesh& mesh,
        const Vec3& start,
        const Vec3& end,
        float radiusStart,
        float radiusEnd,
        const Vec3& color,
        int radialSegments = 8
    );

    static void appendLeafBlade(
        HeliosCanopyMesh& mesh,
        const Vec3& petioleStart,
        const Vec3& petioleEnd,
        float leafLength,
        float leafWidth,
        float rollAngleDeg,
        const Vec3& leafColor
    );
};

} // namespace helios
} // namespace vegetation
