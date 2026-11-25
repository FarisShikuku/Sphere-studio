#include "PineTreeType.h"
#include "categories/TreeCategoryBuilders.h"

namespace vegetation {

PineTreeType::PineTreeType() {
    m_branchNumber = 48;
    m_maxRecursionDepth = 4;
    m_branchAngleDeg = 42.0f;
    m_leafCount = 1850;
    m_leafScale = 0.85f;
    m_trunkRadius = 0.52f;
    m_treeHeight = 18.0f;
    m_windFlexibility = 0.15f;
    m_barkColorR = 0.54f; m_barkColorG = 0.32f; m_barkColorB = 0.18f;
    m_leafColorR = 0.14f; m_leafColorG = 0.40f; m_leafColorB = 0.20f;
    m_terminalLeafCount = 2;
    m_realLeafLengthMeters = 0.075f;
}

CrownControlParams PineTreeType::getCrownHullParams() const {
    categories::TreeBuildContext ctx;
    ctx.speciesId = TreeSpeciesId::SCOTS_PINE;
    ctx.treeHeight = m_treeHeight;
    ctx.trunkRadius = m_trunkRadius;
    ctx.leafCount = m_leafCount;
    return categories::computeSpeciesCrownHull(ctx);
}

void PineTreeType::buildTreeStructure(
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    uint64_t seed
) {
    outBranches.clear();
    outLeaves.clear();

    FastRng rng(seed);
    categories::TreeBuildContext ctx;
    ctx.speciesId = TreeSpeciesId::SCOTS_PINE;
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
