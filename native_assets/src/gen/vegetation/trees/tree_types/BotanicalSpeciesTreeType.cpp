// ============================================================================
// BotanicalSpeciesTreeType.cpp - Clean Category Dispatcher for 65 Tree Species
//
// Delegates crown envelope calculation and biological structure synthesis to
// dedicated category modules under trees/tree_types/categories/:
//  - BotanicalCrownHulls.cpp
//  - PalmAndMonocotBuilder.cpp
//  - SucculentAndBaobabBuilder.cpp
//  - SavannaAndBanyanBuilder.cpp
//  - ConiferAndSpireBuilder.cpp
//  - BroadleafOrchardAndVineBuilder.cpp
// ============================================================================

#include "BotanicalSpeciesTreeType.h"

namespace vegetation {

BotanicalSpeciesTreeType::BotanicalSpeciesTreeType(TreeSpeciesId id)
    : m_speciesId(id) {
    TreeSpeciesConfig cfg = getPredefinedTreeSpecies(id);
    m_commonName = cfg.commonName;
    m_botanicalName = cfg.botanicalName;
    m_treeHeight = cfg.treeHeightMeters;
    m_trunkRadius = cfg.trunkRadiusMeters;
    m_branchNumber = cfg.branchCount;
    m_leafCount = cfg.leafCount;
    m_branchAngleDeg = cfg.branchAngleMeanDeg;
    m_maxRecursionDepth = cfg.maxRecursionDepth;
    m_barkColorR = cfg.trunkColorR; m_barkColorG = cfg.trunkColorG; m_barkColorB = cfg.trunkColorB;
    m_leafColorR = cfg.leafColorR; m_leafColorG = cfg.leafColorG; m_leafColorB = cfg.leafColorB;
    m_windFlexibility = cfg.windFlexibility;

    // 1. Classify authentic botanical architecture category
    switch (id) {
        case TreeSpeciesId::CANDELABRA_EUPHORBIA:
            m_morphotype = TreeMorphotypeCategory::CANDELABRA_EUPHORBIA;
            break;
        case TreeSpeciesId::DATE_PALM:
        case TreeSpeciesId::COCONUT_PALM:
        case TreeSpeciesId::DOUM_PALM:
            m_morphotype = TreeMorphotypeCategory::PALM_MONOCOT;
            break;
        case TreeSpeciesId::BANANA_TREE:
            m_morphotype = TreeMorphotypeCategory::BANANA_MUSA;
            break;
        case TreeSpeciesId::PAPAYA_TREE:
            m_morphotype = TreeMorphotypeCategory::PAPAYA_PAWPAW;
            break;
        case TreeSpeciesId::AFRICAN_BAOBAB:
            m_morphotype = TreeMorphotypeCategory::BAOBAB_PACHYCAUL;
            break;
        case TreeSpeciesId::DESERT_ROSE:
        case TreeSpeciesId::FRANGIPANI_PLUMERIA:
            m_morphotype = TreeMorphotypeCategory::SUCCULENT_CAUDEX;
            break;
        case TreeSpeciesId::RED_MANGROVE:
            m_morphotype = TreeMorphotypeCategory::MANGROVE_STILT;
            break;
        case TreeSpeciesId::MUGUMO_SACRED_FIG:
        case TreeSpeciesId::SYCAMORE_FIG:
            m_morphotype = TreeMorphotypeCategory::BANYAN_FIG;
            break;
        case TreeSpeciesId::ITALIAN_CYPRESS:
            m_morphotype = TreeMorphotypeCategory::CONIFER_SPIRE;
            m_isFastigiateCypress = true;
            break;
        case TreeSpeciesId::NORWAY_SPRUCE:
        case TreeSpeciesId::SCOTS_PINE:
        case TreeSpeciesId::GIANT_REDWOOD:
        case TreeSpeciesId::GIANT_SEQUOIA:
        case TreeSpeciesId::EUROPEAN_LARCH:
        case TreeSpeciesId::ATLAS_CEDAR:
        case TreeSpeciesId::BEACH_CASUARINA:
            m_morphotype = TreeMorphotypeCategory::CONIFER_SPIRE;
            break;
        case TreeSpeciesId::SAVANNA_ACACIA:
        case TreeSpeciesId::YELLOW_FEVER_TREE:
        case TreeSpeciesId::WHISTLING_THORN:
        case TreeSpeciesId::AFRICAN_BLACKWOOD:
        case TreeSpeciesId::FLAMBOYANT_POINCIANA:
            m_morphotype = TreeMorphotypeCategory::SAVANNA_UMBRELLA;
            break;
        case TreeSpeciesId::CREPE_MYRTLE:
        case TreeSpeciesId::JAPANESE_MAPLE:
        case TreeSpeciesId::GUAVA_TREE:
        case TreeSpeciesId::POMEGRANATE_TREE:
            m_morphotype = TreeMorphotypeCategory::MULTI_STEM_VASE;
            break;
        case TreeSpeciesId::PASSION_FRUIT_STANDARD:
            m_morphotype = TreeMorphotypeCategory::CLIMBING_LIANA;
            break;
        case TreeSpeciesId::WEEPING_WILLOW:
            m_morphotype = TreeMorphotypeCategory::WEEPING_PENDULOUS;
            break;
        default:
            m_morphotype = TreeMorphotypeCategory::DECURRENT_BROADLEAF;
            break;
    }

    // 2. Calibrate natural terminal leaf count (1 = alternate/spiral apex, 2 = opposite/decussate pair)
    // and mature blade length in meters for authentic foliage proportions.
    switch (id) {
        case TreeSpeciesId::RED_MAPLE:
        case TreeSpeciesId::SUGAR_MAPLE:
            m_terminalLeafCount = 2;
            m_realLeafLengthMeters = 0.135f;
            break;
        case TreeSpeciesId::JAPANESE_MAPLE:
            m_terminalLeafCount = 2;
            m_realLeafLengthMeters = 0.095f;
            break;
        case TreeSpeciesId::SAVANNA_ACACIA:
        case TreeSpeciesId::YELLOW_FEVER_TREE:
            m_terminalLeafCount = 2;
            m_realLeafLengthMeters = 0.170f;
            break;
        case TreeSpeciesId::WHISTLING_THORN:
            m_terminalLeafCount = 2;
            m_realLeafLengthMeters = 0.135f;
            break;
        case TreeSpeciesId::AFRICAN_BLACKWOOD:
            m_terminalLeafCount = 2;
            m_realLeafLengthMeters = 0.145f;
            break;
        case TreeSpeciesId::FLAMBOYANT_POINCIANA:
            m_terminalLeafCount = 2;
            m_realLeafLengthMeters = 0.195f;
            break;
        case TreeSpeciesId::GOLDEN_SHOWER_CASSIA:
            m_terminalLeafCount = 2;
            m_realLeafLengthMeters = 0.160f;
            break;
        case TreeSpeciesId::JACARANDA:
            m_terminalLeafCount = 2;
            m_realLeafLengthMeters = 0.180f;
            break;
        case TreeSpeciesId::NANDI_FLAME:
        case TreeSpeciesId::SAUSAGE_TREE:
            m_terminalLeafCount = 2;
            m_realLeafLengthMeters = 0.195f;
            break;
        case TreeSpeciesId::YELLOW_TRUMPET_TREE:
            m_terminalLeafCount = 2;
            m_realLeafLengthMeters = 0.150f;
            break;
        case TreeSpeciesId::MEDITERRANEAN_OLIVE:
        case TreeSpeciesId::AFRICAN_WILD_OLIVE:
            m_terminalLeafCount = 2;
            m_realLeafLengthMeters = 0.088f;
            break;
        case TreeSpeciesId::FLOWERING_DOGWOOD:
            m_terminalLeafCount = 2;
            m_realLeafLengthMeters = 0.118f;
            break;
        case TreeSpeciesId::CREPE_MYRTLE:
        case TreeSpeciesId::POMEGRANATE_TREE:
            m_terminalLeafCount = 2;
            m_realLeafLengthMeters = 0.084f;
            break;
        case TreeSpeciesId::GUAVA_TREE:
            m_terminalLeafCount = 2;
            m_realLeafLengthMeters = 0.132f;
            break;
        case TreeSpeciesId::CANDELABRA_EUPHORBIA:
            m_terminalLeafCount = 2;
            m_realLeafLengthMeters = 0.110f;
            break;

        case TreeSpeciesId::COCONUT_PALM:
            m_terminalLeafCount = 1;
            m_realLeafLengthMeters = 1.75f;
            break;
        case TreeSpeciesId::DATE_PALM:
            m_terminalLeafCount = 1;
            m_realLeafLengthMeters = 1.50f;
            break;

        case TreeSpeciesId::SILVER_BIRCH:
        case TreeSpeciesId::GINKGO_BILOBA:
        case TreeSpeciesId::DESERT_ROSE:
        case TreeSpeciesId::BOUGAINVILLEA_STANDARD:
            m_terminalLeafCount = 1;
            m_realLeafLengthMeters = 0.095f;
            break;
        case TreeSpeciesId::EUROPEAN_BEECH:
        case TreeSpeciesId::JAPANESE_CHERRY:
        case TreeSpeciesId::CRIMSON_BOTTLEBRUSH:
        case TreeSpeciesId::HONEYCRISP_APPLE:
        case TreeSpeciesId::SWEET_PEAR:
        case TreeSpeciesId::SWEET_ORANGE:
        case TreeSpeciesId::LEMON_TREE:
            m_terminalLeafCount = 1;
            m_realLeafLengthMeters = 0.112f;
            break;
        case TreeSpeciesId::WEEPING_WILLOW:
            m_terminalLeafCount = 1;
            m_realLeafLengthMeters = 0.125f;
            break;
        case TreeSpeciesId::RED_MANGROVE:
        case TreeSpeciesId::PASSION_FRUIT_STANDARD:
        case TreeSpeciesId::LYCHEE_TREE:
            m_terminalLeafCount = 1;
            m_realLeafLengthMeters = 0.138f;
            break;
        case TreeSpeciesId::ENGLISH_OAK:
        case TreeSpeciesId::MERU_OAK:
        case TreeSpeciesId::MUGUMO_SACRED_FIG:
        case TreeSpeciesId::SYCAMORE_FIG:
        case TreeSpeciesId::PEACH_TREE:
        case TreeSpeciesId::CAPE_CHESTNUT:
            m_terminalLeafCount = 1;
            m_realLeafLengthMeters = 0.150f;
            break;
        case TreeSpeciesId::SWEET_CHESTNUT:
        case TreeSpeciesId::MACADAMIA_TREE:
        case TreeSpeciesId::CASHEW_TREE:
            m_terminalLeafCount = 1;
            m_realLeafLengthMeters = 0.165f;
            break;
        case TreeSpeciesId::AVOCADO_TREE:
        case TreeSpeciesId::JACKFRUIT_TREE:
            m_terminalLeafCount = 1;
            m_realLeafLengthMeters = 0.180f;
            break;
        case TreeSpeciesId::AFRICAN_BAOBAB:
        case TreeSpeciesId::MANGO_TREE:
        case TreeSpeciesId::SOUTHERN_MAGNOLIA:
        case TreeSpeciesId::FRANGIPANI_PLUMERIA:
            m_terminalLeafCount = 1;
            m_realLeafLengthMeters = 0.210f;
            break;
        case TreeSpeciesId::CACAO_TREE:
            m_terminalLeafCount = 1;
            m_realLeafLengthMeters = 0.240f;
            break;
        case TreeSpeciesId::BREADFRUIT_TREE:
            m_terminalLeafCount = 1;
            m_realLeafLengthMeters = 0.340f;
            break;
        default:
            m_terminalLeafCount = 1;
            m_realLeafLengthMeters = 0.130f;
            break;
    }
}

categories::TreeBuildContext BotanicalSpeciesTreeType::makeBuildContext() const {
    categories::TreeBuildContext ctx;
    ctx.speciesId = m_speciesId;
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
    ctx.isFastigiateCypress = m_isFastigiateCypress;
    ctx.isMangroveStilt = (m_morphotype == TreeMorphotypeCategory::MANGROVE_STILT);
    return ctx;
}

CrownControlParams BotanicalSpeciesTreeType::getCrownHullParams() const {
    return categories::computeSpeciesCrownHull(makeBuildContext());
}

void BotanicalSpeciesTreeType::buildTreeStructure(
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    uint64_t seed
) {
    outBranches.clear();
    outLeaves.clear();

    FastRng rng(seed);
    categories::TreeBuildContext ctx = makeBuildContext();

    switch (m_morphotype) {
        case TreeMorphotypeCategory::CANDELABRA_EUPHORBIA:
            categories::buildEuphorbiaCandelabra(ctx, outBranches, outLeaves, rng);
            break;
        case TreeMorphotypeCategory::PALM_MONOCOT:
            categories::buildPalmArchitecture(ctx, outBranches, outLeaves, rng);
            break;
        case TreeMorphotypeCategory::BANANA_MUSA:
            categories::buildBananaPlant(ctx, outBranches, outLeaves, rng);
            break;
        case TreeMorphotypeCategory::PAPAYA_PAWPAW:
            categories::buildPapayaTree(ctx, outBranches, outLeaves, rng);
            break;
        case TreeMorphotypeCategory::BAOBAB_PACHYCAUL:
            categories::buildBaobabPachycaul(ctx, outBranches, outLeaves, rng);
            break;
        case TreeMorphotypeCategory::SUCCULENT_CAUDEX:
            categories::buildDesertRoseAndFrangipani(ctx, outBranches, outLeaves, rng);
            break;
        case TreeMorphotypeCategory::MANGROVE_STILT:
            categories::buildMangroveStiltRoots(ctx, outBranches, outLeaves, rng);
            break;
        case TreeMorphotypeCategory::BANYAN_FIG:
            categories::buildBanyanStranglerFig(ctx, outBranches, outLeaves, rng);
            break;
        case TreeMorphotypeCategory::CONIFER_SPIRE:
            categories::buildConiferSpire(ctx, outBranches, outLeaves, rng);
            break;
        case TreeMorphotypeCategory::SAVANNA_UMBRELLA:
            categories::buildSavannaUmbrella(ctx, outBranches, outLeaves, rng);
            break;
        case TreeMorphotypeCategory::MULTI_STEM_VASE:
            categories::buildMultiStemOrnamental(ctx, outBranches, outLeaves, rng);
            break;
        case TreeMorphotypeCategory::CLIMBING_LIANA:
            categories::buildClimbingVine(ctx, outBranches, outLeaves, rng);
            break;
        case TreeMorphotypeCategory::WEEPING_PENDULOUS:
            categories::buildWeepingPendulous(ctx, outBranches, outLeaves, rng);
            break;
        case TreeMorphotypeCategory::DECURRENT_BROADLEAF:
        default:
            categories::buildDecurrentBroadleaf(ctx, outBranches, outLeaves, rng);
            break;
    }
}

} // namespace vegetation
