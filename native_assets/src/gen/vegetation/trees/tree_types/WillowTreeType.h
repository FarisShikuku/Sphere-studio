#pragma once
// ============================================================================
// WillowTreeType.h - Salix babylonica Weeping Willow Tree Type
// ============================================================================

#include "TreeTypeBase.h"

namespace vegetation {

class WillowTreeType : public TreeTypeBase {
public:
    WillowTreeType();

    TreeSpeciesId getSpeciesId() const override { return TreeSpeciesId::WEEPING_WILLOW; }
    const char* getSpeciesName() const override { return "Weeping Willow"; }
    const char* getBotanicalName() const override { return "Salix babylonica"; }

    void buildTreeStructure(
        std::vector<TreeBranchSegment>& outBranches,
        std::vector<TreeLeafData>& outLeaves,
        uint64_t seed = 42
    ) override;
};

} // namespace vegetation
