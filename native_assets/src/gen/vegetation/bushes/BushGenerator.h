#pragma once
// ============================================================================
// BushGenerator.h - Multi-Stem 3D Shrub, Woody Cane & Botanical Foliage Engine
// ============================================================================

#include "BushParams.h"
#include "../common/VegetationMath.h"
#include <vector>

namespace vegetation {

struct BushMeshData {
    std::vector<VegVertex_C> vertices;
    std::vector<uint32_t> indices;
    Vec3 aabbMin;
    Vec3 aabbMax;
};

struct ShrubBranchSegment3D {
    Vec3 startPos;
    Vec3 endPos;
    float radiusBottom;
    float radiusTop;
    int32_t order;        // 0 = Basal cane/caudex, 1 = Secondary bough, 2 = Twig, 3 = Terminal shoot
    float windWeight;
};

struct ShrubLeafNode3D {
    Vec3 twigAttachment;
    Vec3 midribDir;       // Normalized 3D midrib growth orientation
    float sizeMeters;
    int32_t parentBranchIndex;
    float colorTint;      // 0.0 = interior mature dark leaf, 1.0 = sunlit outer/flush tip
};

enum class ShrubAccentKind : int32_t {
    NONE = 0,
    BERRIES = 1,            // Paired coffee cherries / drupes along axils
    BOUGAINVILLEA_BRACT = 2,// Triple magenta papery bracts + cream center tube
    FLOWER_BLOSSOM = 3,     // 5-petal tropical blossom / flower head
    ACACIA_PUFF = 4,        // Golden globular mimosoid pompom + paired thorns
    THORN_SPINE = 5,        // Paired woody stipular thorns / agave terminal spine
    MOPHEAD_CORYMB = 6,     // Multi-floret Hydrangea hemispherical flower head
    FERN_FIDDLEHEAD = 7,    // Coiled circinate vernation crosier in fern crown
    PROTEA_CUP = 8,         // King Protea layered involucral bract bowl
    TORCH_SPIKE = 9,        // Flame-red Aloe / Ocotillo terminal floral raceme
    WINGED_FRUIT = 10       // Desert saltbush 4-winged bract / cactus glochids
};

struct ShrubAccentNode3D {
    Vec3 position;
    Vec3 dir;
    float sizeMeters;
    ShrubAccentKind kind;
    int32_t parentBranchIndex;
    float phaseOffset;
};

struct ShrubStructureResult {
    std::vector<ShrubBranchSegment3D> branches;
    std::vector<ShrubLeafNode3D> leaves;
    std::vector<ShrubAccentNode3D> accents;
    int32_t caneCount{0};
    int32_t twigCount{0};
};

class BushGenerator {
public:
    explicit BushGenerator(const BushSynthesisParams& params);

    BushMeshData generateBushMesh(uint64_t seed);
    ShrubStructureResult synthesizeShrubStructure(uint64_t seed) const;

private:
    BushSynthesisParams m_params;
};

} // namespace vegetation
