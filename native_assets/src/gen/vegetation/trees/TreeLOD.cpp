// ============================================================================
// TreeLOD.cpp - Level of Detail Transition Math
// ============================================================================

#include "TreeLOD.h"
#include <algorithm>

namespace vegetation {

TreeLODTier TreeLOD::evaluateLODTier(float cameraDistanceMeters, const TreeLODDistances& dists) {
    if (cameraDistanceMeters < dists.lod0MaxDist) return TreeLODTier::LOD0_FULL_BRANCHES_AND_LEAVES;
    if (cameraDistanceMeters < dists.lod1MaxDist) return TreeLODTier::LOD1_SIMPLIFIED_CARDS;
    if (cameraDistanceMeters < dists.lod2MaxDist) return TreeLODTier::LOD2_TRUNK_AND_HULL;
    return TreeLODTier::LOD3_BILLBOARD_IMPOSTOR;
}

float TreeLOD::computeMorphAlpha(float cameraDistanceMeters, TreeLODTier currentTier, const TreeLODDistances& dists) {
    float start = 0.0f, end = 1.0f;
    switch (currentTier) {
        case TreeLODTier::LOD0_FULL_BRANCHES_AND_LEAVES:
            start = dists.lod0MaxDist * 0.8f;
            end = dists.lod0MaxDist;
            break;
        case TreeLODTier::LOD1_SIMPLIFIED_CARDS:
            start = dists.lod1MaxDist * 0.8f;
            end = dists.lod1MaxDist;
            break;
        case TreeLODTier::LOD2_TRUNK_AND_HULL:
            start = dists.lod2MaxDist * 0.8f;
            end = dists.lod2MaxDist;
            break;
        case TreeLODTier::LOD3_BILLBOARD_IMPOSTOR:
            return 1.0f;
    }
    float t = (cameraDistanceMeters - start) / (end - start);
    return std::clamp(t, 0.0f, 1.0f);
}

} // namespace vegetation
