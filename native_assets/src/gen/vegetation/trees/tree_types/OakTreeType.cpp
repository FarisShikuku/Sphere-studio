#include "OakTreeType.h"
#include "categories/TreeCategoryBuilders.h"

namespace vegetation {

OakTreeType::OakTreeType() {
    m_branchNumber = 48;
    m_maxRecursionDepth = 4;
    m_branchAngleDeg = 42.0f;
    m_leafCount = 1650;
    m_leafScale = 1.25f;
    m_trunkRadius = 0.68f;
    m_treeHeight = 16.5f;
    m_windFlexibility = 0.22f;
    m_barkColorR = 0.36f; m_barkColorG = 0.25f; m_barkColorB = 0.16f;
    m_leafColorR = 0.16f; m_leafColorG = 0.46f; m_leafColorB = 0.15f;
    m_terminalLeafCount = 1;
    m_realLeafLengthMeters = 0.145f;
}

CrownControlParams OakTreeType::getCrownHullParams() const {
    categories::TreeBuildContext ctx;
    ctx.speciesId = TreeSpeciesId::ENGLISH_OAK;
    ctx.treeHeight = m_treeHeight;
    ctx.trunkRadius = m_trunkRadius;
    ctx.leafCount = m_leafCount;
    return categories::computeSpeciesCrownHull(ctx);
}

void OakTreeType::buildTreeStructure(
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    uint64_t seed
) {
    outBranches.clear();
    outLeaves.clear();

    FastRng rng(seed);
    categories::TreeBuildContext ctx;
    ctx.speciesId = TreeSpeciesId::ENGLISH_OAK;
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

    categories::buildDecurrentBroadleaf(ctx, outBranches, outLeaves, rng);
}

} // namespace vegetation
