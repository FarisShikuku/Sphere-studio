#pragma once
// ============================================================================
// AcaciaTreeType.h - Acacia tortilis Savanna Umbrella Tree Type
// ============================================================================

#include "TreeTypeBase.h"

namespace vegetation {

class AcaciaTreeType : public TreeTypeBase {
public:
    AcaciaTreeType();

    TreeSpeciesId getSpeciesId() const override { return TreeSpeciesId::SAVANNA_ACACIA; }
    const char* getSpeciesName() const override { return "Savanna Acacia"; }
    const char* getBotanicalName() const override { return "Vachellia tortilis"; }

    CrownControlParams getCrownHullParams() const override;

    void buildTreeStructure(
        std::vector<TreeBranchSegment>& outBranches,
        std::vector<TreeLeafData>& outLeaves,
        uint64_t seed = 42
    ) override;
};

} // namespace vegetation
