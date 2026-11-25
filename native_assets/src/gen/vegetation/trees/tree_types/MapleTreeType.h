#pragma once
// ============================================================================
// MapleTreeType.h - Acer saccharum Sugar Maple Tree Type
// ============================================================================

#include "TreeTypeBase.h"

namespace vegetation {

class MapleTreeType : public TreeTypeBase {
public:
    MapleTreeType();

    TreeSpeciesId getSpeciesId() const override { return TreeSpeciesId::SUGAR_MAPLE; }
    const char* getSpeciesName() const override { return "Sugar Maple"; }
    const char* getBotanicalName() const override { return "Acer saccharum"; }

    void buildTreeStructure(
        std::vector<TreeBranchSegment>& outBranches,
        std::vector<TreeLeafData>& outLeaves,
        uint64_t seed = 42
    ) override;
};

} // namespace vegetation
