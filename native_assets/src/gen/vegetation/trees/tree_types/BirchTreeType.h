#pragma once
// ============================================================================
// BirchTreeType.h - Betula pendula Silver Birch Tree Type
// ============================================================================

#include "TreeTypeBase.h"

namespace vegetation {

class BirchTreeType : public TreeTypeBase {
public:
    BirchTreeType();

    TreeSpeciesId getSpeciesId() const override { return TreeSpeciesId::SILVER_BIRCH; }
    const char* getSpeciesName() const override { return "Silver Birch"; }
    const char* getBotanicalName() const override { return "Betula pendula"; }

    CrownControlParams getCrownHullParams() const override;

    void buildTreeStructure(
        std::vector<TreeBranchSegment>& outBranches,
        std::vector<TreeLeafData>& outLeaves,
        uint64_t seed = 42
    ) override;
};

} // namespace vegetation
