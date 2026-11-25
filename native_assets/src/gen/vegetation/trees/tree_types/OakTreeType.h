#pragma once
// ============================================================================
// OakTreeType.h - Quercus robur Broadleaf Decurrent Tree Type
// ============================================================================

#include "TreeTypeBase.h"

namespace vegetation {

class OakTreeType : public TreeTypeBase {
public:
    OakTreeType();

    TreeSpeciesId getSpeciesId() const override { return TreeSpeciesId::ENGLISH_OAK; }
    const char* getSpeciesName() const override { return "English Oak"; }
    const char* getBotanicalName() const override { return "Quercus robur"; }

    CrownControlParams getCrownHullParams() const override;

    void buildTreeStructure(
        std::vector<TreeBranchSegment>& outBranches,
        std::vector<TreeLeafData>& outLeaves,
        uint64_t seed = 42
    ) override;
};

} // namespace vegetation
