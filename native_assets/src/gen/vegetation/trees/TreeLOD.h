#pragma once
// ============================================================================
// TreeLOD.h - Discrete Level of Detail Evaluator for Botanical Models
// ============================================================================

#include <cstdint>
#include "../common/VegetationMath.h"

namespace vegetation {

enum class TreeLODTier : uint32_t {
    LOD0_FULL_BRANCHES_AND_LEAVES = 0,
    LOD1_SIMPLIFIED_CARDS = 1,
    LOD2_TRUNK_AND_HULL = 2,
    LOD3_BILLBOARD_IMPOSTOR = 3
};

struct TreeLODDistances {
    float lod0MaxDist{35.0f};
    float lod1MaxDist{80.0f};
    float lod2MaxDist{160.0f};
    float lod3MaxDist{400.0f};
};

class TreeLOD {
public:
    static TreeLODTier evaluateLODTier(float cameraDistanceMeters, const TreeLODDistances& dists);
    static float computeMorphAlpha(float cameraDistanceMeters, TreeLODTier currentTier, const TreeLODDistances& dists);
};

} // namespace vegetation
