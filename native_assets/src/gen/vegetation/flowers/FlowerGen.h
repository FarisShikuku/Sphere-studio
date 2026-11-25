#pragma once
// ============================================================================
// FlowerGen.h - 3D Corolla, Petals, Leaves & Floral Clump Engine in C++17
// ============================================================================

#include "FlowerParams.h"
#include "../common/VegetationMath.h"
#include <vector>

namespace vegetation {

struct FlowerMeshData {
    std::vector<VegVertex_C> vertices;
    std::vector<uint32_t> indices;
};

struct FlowerStemSegment3D {
    Vec3 startPos;
    Vec3 endPos;
    float radiusBottom;
    float radiusTop;
    int32_t kind;       // 0 = main stem, 1 = pedicel/spikelet, 2 = bract spathe
    float windWeight;
};

struct FlowerLeaf3D {
    Vec3 attachPos;
    Vec3 midribDir;
    Vec3 wingDir;
    float lengthMeters;
    float widthMeters;
    float tint;
    float windWeight;
};

struct FlowerPetalQuad3D {
    Vec3 p0;            // Base inner corner
    Vec3 p1;            // Mid-left flared corner
    Vec3 p2;            // Outer petal tip
    Vec3 p3;            // Mid-right flared corner
    float colorR;
    float colorG;
    float colorB;
    float shade;
    float windWeight;
};

struct FlowerDiskHead3D {
    Vec3 centerPos;
    Vec3 normalDir;
    float radiusMeters;
    float coneHeightMeters;
    int32_t styleKind;  // 0 = flat/domed composite disc, 1 = raised echinacea cone, 2 = poppy stamen boss, 3 = bell/spike floret, 4 = strelitzia crest
    float colorR;
    float colorG;
    float colorB;
    float windWeight;
};

struct FlowerClusterResult {
    std::vector<FlowerStemSegment3D> stems;
    std::vector<FlowerLeaf3D> leaves;
    std::vector<FlowerPetalQuad3D> petalQuads;
    std::vector<FlowerDiskHead3D> diskHeads;
    int32_t totalBlossomHeads{0};
};

class FlowerGen {
public:
    static FlowerMeshData generateFlower(const FlowerSynthesisParams& params, uint64_t seed);
    static FlowerClusterResult synthesizeFlowerCluster(const FlowerSynthesisParams& params, uint64_t seed);
};

} // namespace vegetation
