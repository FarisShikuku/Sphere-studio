#pragma once
// ============================================================================
// CropRowGen.h - 3D Agricultural Furrow, Stalk, Leaf & Harvest Engine in C++17
// ============================================================================

#include "CropParams.h"
#include "../common/VegetationMath.h"
#include <vector>

namespace vegetation {

struct CropFieldMesh {
    std::vector<VegVertex_C> furrowVertices;
    std::vector<uint32_t> furrowIndices;
    std::vector<VegVec3_C> plantStalkPositions;
    uint32_t totalStalkCount{0};
};

struct CropFurrowQuad3D {
    Vec3 p0, p1, p2, p3;
    float moistureShade; // 0 = dry sun-warmed ridge crest, 1 = moist dark furrow trough
};

struct CropStalkSegment3D {
    Vec3 startPos;
    Vec3 endPos;
    float radiusBottom;
    float radiusTop;
    int32_t kind;        // 0 = green/gold culm or pseudostem, 1 = woody cane/branch, 2 = trellis post, 3 = trellis wire
    float windWeight;
};

struct CropLeafBlade3D {
    Vec3 attachPos;
    Vec3 midribDir;
    Vec3 wingDir;
    float lengthMeters;
    float widthMeters;
    float tint;          // 0 = shaded lower leaf, 1 = bright sunlit canopy/flush leaf
    float windWeight;
};

enum class CropOrganKind : int32_t {
    MAIZE_EAR_COB = 0,
    MAIZE_TASSEL = 1,
    BANANA_HAND_BUNCH = 2,
    COFFEE_CHERRY_CLUSTER = 3,
    SUGARCANE_PLUME = 4,
    WHEAT_GRAIN_SPIKE = 5,
    GRAPE_CLUSTER = 6,
    POTATO_BLOSSOM = 7,
    TEA_FLUSH_SHOOT = 8,
    CASSAVA_NODE_BUD = 9
};

struct CropHarvestOrgan3D {
    Vec3 position;
    Vec3 dir;
    float sizeMeters;
    CropOrganKind kind;
    float windWeight;
};

struct CropSceneResult {
    std::vector<CropFurrowQuad3D> furrowQuads;
    std::vector<CropStalkSegment3D> stalks;
    std::vector<CropLeafBlade3D> leaves;
    std::vector<CropHarvestOrgan3D> organs;
    int32_t totalPlants{0};
    int32_t rowCount{0};
};

class CropRowGen {
public:
    static CropFieldMesh generateCropField(const CropSynthesisParams& params, uint64_t seed);
    static CropSceneResult synthesizeCropScene(const CropSynthesisParams& params, uint64_t seed);
};

} // namespace vegetation
