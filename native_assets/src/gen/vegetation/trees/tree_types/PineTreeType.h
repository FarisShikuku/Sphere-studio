#pragma once
// ============================================================================
// PineTreeType.h - Pinus sylvestris Conifer / Scots Pine Tree Type
// ============================================================================

#include "TreeTypeBase.h"

namespace vegetation {

class PineTreeType : public TreeTypeBase {
public:
    PineTreeType();

    TreeSpeciesId getSpeciesId() const override { return TreeSpeciesId::SCOTS_PINE; }
    const char* getSpeciesName() const override { return "Scots Pine"; }
    const char* getBotanicalName() const override { return "Pinus sylvestris"; }

    CrownControlParams getCrownHullParams() const override;

    void buildTreeStructure(
        std::vector<TreeBranchSegment>& outBranches,
        std::vector<TreeLeafData>& outLeaves,
        uint64_t seed = 42
    ) override;
};

} // namespace vegetation
