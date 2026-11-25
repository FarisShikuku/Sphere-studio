#include "AcaciaTreeType.h"
#include "categories/TreeCategoryBuilders.h"

namespace vegetation {

AcaciaTreeType::AcaciaTreeType() {
    m_branchNumber = 48;
    m_maxRecursionDepth = 3;
    m_branchAngleDeg = 85.0f;
    m_leafCount = 1400;
    m_leafScale = 1.10f;
    m_trunkRadius = 0.44f;
    m_treeHeight = 11.0f;
    m_windFlexibility = 0.22f;
    m_barkColorR = 0.35f; m_barkColorG = 0.25f; m_barkColorB = 0.18f;
    m_leafColorR = 0.38f; m_leafColorG = 0.60f; m_leafColorB = 0.18f;
    m_terminalLeafCount = 2;
    m_realLeafLengthMeters = 0.17f;
}

CrownControlParams AcaciaTreeType::getCrownHullParams() const {
    categories::TreeBuildContext ctx;
    ctx.speciesId = TreeSpeciesId::SAVANNA_ACACIA;
    ctx.treeHeight = m_treeHeight;
    ctx.trunkRadius = m_trunkRadius;
    ctx.leafCount = m_leafCount;
    return categories::computeSpeciesCrownHull(ctx);
}

void AcaciaTreeType::buildTreeStructure(
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    uint64_t seed
) {
    outBranches.clear();
    outLeaves.clear();

    FastRng rng(seed);
    categories::TreeBuildContext ctx;
    ctx.speciesId = TreeSpeciesId::SAVANNA_ACACIA;
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

    categories::buildSavannaUmbrella(ctx, outBranches, outLeaves, rng);
}

} // namespace vegetation
