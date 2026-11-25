#pragma once
// ============================================================================
// TreeSpecies.h - Tree Taxonomy & Growth Parametrization in C++17
// ============================================================================

#include <string>
#include <vector>
#include <cstdint>

namespace vegetation {

enum class TreeSpeciesId : uint32_t {
    ENGLISH_OAK = 0,
    SCOTS_PINE = 1,
    SILVER_BIRCH = 2,
    DATE_PALM = 3,
    WEEPING_WILLOW = 4,
    SAVANNA_ACACIA = 5,
    GIANT_REDWOOD = 6,
    SUGAR_MAPLE = 7,
    NORWAY_SPRUCE = 8,
    GIANT_SEQUOIA = 9,
    EUROPEAN_BEECH = 10,
    GINKGO_BILOBA = 11,
    MEDITERRANEAN_OLIVE = 12,
    SWEET_CHESTNUT = 13,
    EUROPEAN_LARCH = 14,
    JAPANESE_CHERRY = 15,
    ATLAS_CEDAR = 16,
    RED_MAPLE = 17,
    AFRICAN_BAOBAB = 18,
    YELLOW_FEVER_TREE = 19,
    MUGUMO_SACRED_FIG = 20,
    AFRICAN_BLACKWOOD = 21,
    NANDI_FLAME = 22,
    SYCAMORE_FIG = 23,
    WHISTLING_THORN = 24,
    AFRICAN_WILD_OLIVE = 25,
    MERU_OAK = 26,
    SAUSAGE_TREE = 27,
    COCONUT_PALM = 28,
    RED_MANGROVE = 29,
    BEACH_CASUARINA = 30,
    DOUM_PALM = 31,
    DESERT_ROSE = 32,
    CANDELABRA_EUPHORBIA = 33,
    BANANA_TREE = 34,
    MANGO_TREE = 35,
    PAPAYA_TREE = 36,
    JACARANDA = 37,
    FLAMBOYANT_POINCIANA = 38,
    YELLOW_TRUMPET_TREE = 39,
    SOUTHERN_MAGNOLIA = 40,
    CREPE_MYRTLE = 41,
    GOLDEN_SHOWER_CASSIA = 42,
    FRANGIPANI_PLUMERIA = 43,
    CRIMSON_BOTTLEBRUSH = 44,
    CAPE_CHESTNUT = 45,
    ITALIAN_CYPRESS = 46,
    JAPANESE_MAPLE = 47,
    BOUGAINVILLEA_STANDARD = 48,
    FLOWERING_DOGWOOD = 49,
    AVOCADO_TREE = 50,
    SWEET_ORANGE = 51,
    LEMON_TREE = 52,
    MACADAMIA_TREE = 53,
    JACKFRUIT_TREE = 54,
    GUAVA_TREE = 55,
    PASSION_FRUIT_STANDARD = 56,
    POMEGRANATE_TREE = 57,
    HONEYCRISP_APPLE = 58,
    SWEET_PEAR = 59,
    PEACH_TREE = 60,
    CACAO_TREE = 61,
    BREADFRUIT_TREE = 62,
    LYCHEE_TREE = 63,
    CASHEW_TREE = 64
};

struct TreeSpeciesConfig {
    TreeSpeciesId speciesId{TreeSpeciesId::ENGLISH_OAK};
    std::string commonName{"English Oak"};
    std::string botanicalName{"Quercus robur"};
    std::string lsystemAxiom{"A"};
    std::string lsystemRules{"A=F[&+B][&-B][^+B][^-B]FA;B=F[+B][-B]FB"};
    int maxRecursionDepth{4};
    float branchAngleMeanDeg{28.5f};
    float branchAngleDeviationDeg{6.0f};
    float branchLengthDecay{0.72f};
    float trunkRadiusMeters{0.45f};
    float radiusDecay{0.68f};
    float treeHeightMeters{12.0f};
    float leafClusterSizeMeters{0.85f};
    float trunkColorR{0.36f}, trunkColorG{0.25f}, trunkColorB{0.18f};
    float leafColorR{0.22f}, leafColorG{0.48f}, leafColorB{0.16f};
    float windFlexibility{0.35f};
    int branchCount{24};
    int leafCount{260};
    float stemLengthRatio{0.35f};
};

TreeSpeciesConfig getPredefinedTreeSpecies(TreeSpeciesId id);

} // namespace vegetation
