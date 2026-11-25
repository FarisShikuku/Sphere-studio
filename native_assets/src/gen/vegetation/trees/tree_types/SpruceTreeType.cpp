#include "SpruceTreeType.h"
#include "categories/TreeCategoryBuilders.h"

namespace vegetation {

SpruceTreeType::SpruceTreeType() {
    m_branchNumber = 54;
    m_maxRecursionDepth = 4;
    m_branchAngleDeg = 65.0f;
    m_leafCount = 2200;
    m_leafScale = 0.70f;
    m_trunkRadius = 0.45f;
    m_treeHeight = 20.0f;
    m_windFlexibility = 0.18f;
    m_barkColorR = 0.38f; m_barkColorG = 0.28f; m_barkColorB = 0.22f;
    m_leafColorR = 0.10f; m_leafColorG = 0.38f; m_leafColorB = 0.16f;
    m_terminalLeafCount = 1;
    m_realLeafLengthMeters = 0.045f;
}

CrownControlParams SpruceTreeType::getCrownHullParams() const {
    categories::TreeBuildContext ctx;
    ctx.speciesId = TreeSpeciesId::NORWAY_SPRUCE;
    ctx.treeHeight = m_treeHeight;
    ctx.trunkRadius = m_trunkRadius;
    ctx.leafCount = m_leafCount;
    return categories::computeSpeciesCrownHull(ctx);
}

void SpruceTreeType::buildTreeStructure(
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    uint64_t seed
) {
    outBranches.clear();
    outLeaves.clear();

    FastRng rng(seed);
    categories::TreeBuildContext ctx;
    ctx.speciesId = TreeSpeciesId::NORWAY_SPRUCE;
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

    categories::buildConiferSpire(ctx, outBranches, outLeaves, rng);
}

} // namespace vegetation
