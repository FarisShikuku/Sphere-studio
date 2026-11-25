#pragma once
// ============================================================================
// BotanicalSpeciesTreeType.h - Modular Dispatcher for 65 Botanical & Fruit Species
//
// Routes each species to its dedicated biological category module under
// trees/tree_types/categories/ along with calibrated PlantGL Asymmetric Crown
// Hulls, real-world clear trunk boles, and Stojanovic (2016) acropetal foliage.
// ============================================================================

#include "TreeTypeBase.h"
#include "categories/TreeCategoryBuilders.h"

namespace vegetation {

enum class TreeMorphotypeCategory {
    CANDELABRA_EUPHORBIA,
    PALM_MONOCOT,
    BANANA_MUSA,
    PAPAYA_PAWPAW,
    BAOBAB_PACHYCAUL,
    SUCCULENT_CAUDEX,
    SAVANNA_UMBRELLA,
    MANGROVE_STILT,
    BANYAN_FIG,
    CONIFER_SPIRE,
    WEEPING_PENDULOUS,
    MULTI_STEM_VASE,
    CLIMBING_LIANA,
    DECURRENT_BROADLEAF
};

class BotanicalSpeciesTreeType : public TreeTypeBase {
public:
    explicit BotanicalSpeciesTreeType(TreeSpeciesId id);

    TreeSpeciesId getSpeciesId() const override { return m_speciesId; }
    const char* getSpeciesName() const override { return m_commonName.c_str(); }
    const char* getBotanicalName() const override { return m_botanicalName.c_str(); }

    CrownControlParams getCrownHullParams() const override;

    void buildTreeStructure(
        std::vector<TreeBranchSegment>& outBranches,
        std::vector<TreeLeafData>& outLeaves,
        uint64_t seed = 42
    ) override;

private:
    categories::TreeBuildContext makeBuildContext() const;

    TreeSpeciesId m_speciesId;
    std::string m_commonName;
    std::string m_botanicalName;
    TreeMorphotypeCategory m_morphotype{TreeMorphotypeCategory::DECURRENT_BROADLEAF};
    bool m_isFastigiateCypress{false};
};

} // namespace vegetation
