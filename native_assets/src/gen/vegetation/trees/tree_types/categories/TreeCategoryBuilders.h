#pragma once
// ============================================================================
// TreeCategoryBuilders.h
// Modular Morphological Builders for Botanical Tree & Fruit Archetypes in C++17
//
// Categories:
//  1. BotanicalCrownHulls           - PlantGL asymmetric canopy hull envelopes
//  2. PalmAndMonocotBuilder         - Palms (monocaul/doum), Banana, Pawpaw/Papaya
//  3. SucculentAndBaobabBuilder     - Candelabra Euphorbia, Baobab, Desert Rose, Frangipani
//  4. SavannaAndBanyanBuilder       - Flat-top Acacias, Stilt Mangrove, Aerial-root Banyan Figs
//  5. ConiferAndSpireBuilder        - Excurrent whorled conifers & fastigiate Italian Cypress
//  6. BroadleafOrchardAndVineBuilder- Rainforest canopy, Fruit orchards, Multi-stem, Willow, Vine
// ============================================================================

#include "../TreeTypeBase.h"

namespace vegetation {
namespace categories {

struct TreeBuildContext {
    TreeSpeciesId speciesId{TreeSpeciesId::ENGLISH_OAK};
    float treeHeight{14.0f};
    float trunkRadius{0.45f};
    int branchNumber{32};
    int leafCount{360};
    float branchAngleDeg{42.0f};
    float leafScale{1.0f};

    float leafColorR{0.20f};
    float leafColorG{0.48f};
    float leafColorB{0.16f};

    int terminalLeafCount{1};
    float realLeafLengthMeters{0.13f};

    bool isFastigiateCypress{false};
    bool isMangroveStilt{false};
};

// 1. Crown Envelope Geometry (BotanicalCrownHulls.cpp)
CrownControlParams computeSpeciesCrownHull(const TreeBuildContext& ctx);

// 2. Palms & Giant Monocot / Pachycaul Herb Columns (PalmAndMonocotBuilder.cpp)
void buildPalmArchitecture(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
);

void buildBananaPlant(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
);

void buildPapayaTree(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
);

// 3. Succulent Candelabra & Caudex Trees (SucculentAndBaobabBuilder.cpp)
void buildEuphorbiaCandelabra(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
);

void buildBaobabPachycaul(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
);

void buildDesertRoseAndFrangipani(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
);

// 4. Savanna Umbrella Tables, Prop-Root Mangroves & Banyan Figs (SavannaAndBanyanBuilder.cpp)
void buildSavannaUmbrella(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
);

void buildMangroveStiltRoots(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
);

void buildBanyanStranglerFig(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
);

// 5. Monopodial Excurrent Conifers & Fastigiate Spires (ConiferAndSpireBuilder.cpp)
void buildConiferSpire(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
);

// 6. Decurrent Broadleaf, Tropical Rainforest, Orchards, Willow & Climbing Vines
//    (BroadleafOrchardAndVineBuilder.cpp)
void buildDecurrentBroadleaf(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
);

void buildMultiStemOrnamental(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
);

void buildWeepingPendulous(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
);

void buildClimbingVine(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
);

} // namespace categories
} // namespace vegetation
