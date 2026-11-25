#pragma once
#include "DesertSpeciesParams.h"
#include "../common/VegetationMath.h"
#include <vector>
#include <cstdint>

namespace vegetation {
namespace desert {

struct CactusMeshVertex {
    Vec3 position;
    Vec3 normal;
    float u;
    float v;
    uint32_t colorArgb;
};

struct CactusProceduralMesh {
    std::vector<CactusMeshVertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t spineCount{0};
    uint32_t areoleCount{0};
};

using DesertMesh3D = CactusProceduralMesh;

struct DesertQuad3D {
    Vec3 p0;
    Vec3 p1;
    Vec3 p2;
    Vec3 p3;
    Vec3 normal;
    float shade;       // 0.0 (deep flute valley) .. 1.0 (sunlit rib crest)
    float windWeight;
    int32_t partKind;  // 0 = fluted succulent body, 1 = cladode pad, 2 = woody bark, 3 = crown wool/felt
};

struct DesertBranchSeg3D {
    Vec3 startPos;
    Vec3 endPos;
    float radiusBottom;
    float radiusTop;
    int32_t order;
    float windWeight;
};

struct DesertLeafBlade3D {
    Vec3 attachPos;
    Vec3 midribDir;
    Vec3 wingDir;      // True 3D lateral blade axis (non-billboarded)
    float lengthMeters;
    float widthMeters;
    float tint;        // 0.0 = basal shaded blade, 1.0 = sunlit crown blade
    int32_t branchIdx;
};

struct DesertSpine3D {
    Vec3 origin;
    Vec3 dir;
    float lengthMeters;
    float curvature;   // 0.0 = straight needle, >0 = hooked fishhook spine
    float windWeight;
    int32_t style;     // 0 = ivory/amber spine, 1 = wool areole tuft, 2 = white haworthia tubercle band
};

struct DesertBloom3D {
    Vec3 position;
    Vec3 dir;
    float sizeMeters;
    int32_t kind;      // 0 = nocturnal cream blossom, 1 = ruby tuna/fruit, 2 = scarlet raceme torch, 3 = creamy panicle
    float phaseOffset;
};

struct DesertSpecimenResult {
    std::vector<DesertQuad3D> quads;
    std::vector<DesertBranchSeg3D> branches;
    std::vector<DesertLeafBlade3D> leaves;
    std::vector<DesertSpine3D> spines;
    std::vector<DesertBloom3D> blooms;
    uint32_t ribCount{0};
    uint32_t areoleCount{0};
};

class DesertCactusGenerator {
public:
    explicit DesertCactusGenerator(const DesertSpeciesParams& params = {}) : m_params(params) {}

    DesertMesh3D generateMesh(uint32_t seed = 0) const {
        (void)seed;
        return generateCactusMesh(m_params);
    }

    DesertSpecimenResult synthesizeSpecimen(uint64_t seed = 1337ULL) const;

    static CactusProceduralMesh generateCactusMesh(const DesertSpeciesParams& params);

private:
    DesertSpeciesParams m_params;
};

} // namespace desert
} // namespace vegetation
