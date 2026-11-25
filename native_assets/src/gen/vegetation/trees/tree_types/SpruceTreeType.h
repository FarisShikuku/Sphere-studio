#pragma once
// ============================================================================
// SpruceTreeType.h - Picea abies (Norway Spruce) Conical Spire Conifer
// ============================================================================

#include "TreeTypeBase.h"

namespace vegetation {

class SpruceTreeType : public TreeTypeBase {
public:
    SpruceTreeType();

    TreeSpeciesId getSpeciesId() const override { return TreeSpeciesId::NORWAY_SPRUCE; }
    const char* getSpeciesName() const override { return "Norway Spruce"; }
    const char* getBotanicalName() const override { return "Picea abies"; }

    CrownControlParams getCrownHullParams() const override;

    void buildTreeStructure(
        std::vector<TreeBranchSegment>& outBranches,
        std::vector<TreeLeafData>& outLeaves,
        uint64_t seed = 42
    ) override;
};

} // namespace vegetation
