#include "WillowTreeType.h"
#include "categories/TreeCategoryBuilders.h"

namespace vegetation {

WillowTreeType::WillowTreeType() {
    m_branchNumber = 64;
    m_maxRecursionDepth = 4;
    m_branchAngleDeg = 48.0f;
    m_leafCount = 1950;
    m_leafScale = 1.05f;
    m_trunkRadius = 0.62f;
    m_treeHeight = 13.5f;
    m_windFlexibility = 0.85f;
    m_barkColorR = 0.35f; m_barkColorG = 0.28f; m_barkColorB = 0.20f;
    m_leafColorR = 0.25f; m_leafColorG = 0.52f; m_leafColorB = 0.16f;
    m_terminalLeafCount = 1;
    m_realLeafLengthMeters = 0.125f;
}

void WillowTreeType::buildTreeStructure(
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    uint64_t seed
) {
    outBranches.clear();
    outLeaves.clear();

    FastRng rng(seed);
    categories::TreeBuildContext ctx;
    ctx.speciesId = TreeSpeciesId::WEEPING_WILLOW;
    ctx.treeHeight = m_treeHeight;
    ctx.trunkRadius = m_trunkRadius;
    ctx.branchNumber = m_branchNumber;
    ctx.leafCount = m_leafCount;
    ctx.branchAngleDeg = m_branchAngleDeg;
    ctx.leafScale = m_leafScale;
    ctx.leafColorR = m_leafColorR;
    ctx.leafColorG = m_leafColorG;
    ctx.leafColorB = m_leafColorB;
    ctx.terminalLeafCount = m_terminalLeafCount;
    ctx.realLeafLengthMeters = m_realLeafLengthMeters;

    categories::buildWeepingPendulous(ctx, outBranches, outLeaves, rng);
}

} // namespace vegetation
