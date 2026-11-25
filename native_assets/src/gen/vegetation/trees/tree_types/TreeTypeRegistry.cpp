#include "TreeTypeRegistry.h"
#include <algorithm>

namespace vegetation {

std::unique_ptr<TreeTypeBase> TreeTypeRegistry::createTreeType(TreeSpeciesId id) {
    switch (id) {
        case TreeSpeciesId::ENGLISH_OAK:
            return std::make_unique<OakTreeType>();
        case TreeSpeciesId::SCOTS_PINE:
            return std::make_unique<PineTreeType>();
        case TreeSpeciesId::NORWAY_SPRUCE:
            return std::make_unique<SpruceTreeType>();
        case TreeSpeciesId::SILVER_BIRCH:
            return std::make_unique<BirchTreeType>();
        case TreeSpeciesId::WEEPING_WILLOW:
            return std::make_unique<WillowTreeType>();
        case TreeSpeciesId::SUGAR_MAPLE:
            return std::make_unique<MapleTreeType>();
        case TreeSpeciesId::SAVANNA_ACACIA:
            return std::make_unique<AcaciaTreeType>();
        default:
            return std::make_unique<BotanicalSpeciesTreeType>(id);
    }
}

std::unique_ptr<TreeTypeBase> TreeTypeRegistry::createTreeTypeByName(const std::string& name) {
    std::string lower = name;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower.find("spruce") != std::string::npos) return std::make_unique<SpruceTreeType>();
    if (lower.find("scots pine") != std::string::npos) return std::make_unique<PineTreeType>();
    if (lower.find("birch") != std::string::npos) return std::make_unique<BirchTreeType>();
    if (lower.find("willow") != std::string::npos) return std::make_unique<WillowTreeType>();
    if (lower.find("maple") != std::string::npos) return std::make_unique<MapleTreeType>();
    if (lower.find("savanna acacia") != std::string::npos) return std::make_unique<AcaciaTreeType>();
    if (lower.find("english oak") != std::string::npos) return std::make_unique<OakTreeType>();

    // Match any of the 65 species from registry
    auto ids = getAvailableTreeTypeIds();
    for (auto id : ids) {
        TreeSpeciesConfig cfg = getPredefinedTreeSpecies(id);
        std::string cName = cfg.commonName;
        std::transform(cName.begin(), cName.end(), cName.begin(), ::tolower);
        if (cName.find(lower) != std::string::npos || lower.find(cName) != std::string::npos) {
            return createTreeType(id);
        }
    }
    return std::make_unique<BotanicalSpeciesTreeType>(TreeSpeciesId::ENGLISH_OAK);
}

std::vector<std::string> TreeTypeRegistry::getAvailableTreeTypeNames() {
    return {
        "Scots Pine",
        "English Oak",
        "Norway Spruce",
        "Giant Sequoia",
        "Silver Birch",
        "European Beech",
        "Sugar Maple",
        "Weeping Willow",
        "Ginkgo Biloba",
        "Savanna Acacia",
        "Ancient Olive",
        "Sweet Chestnut",
        "European Larch",
        "Japanese Cherry",
        "Atlas Cedar",
        "Red Maple",
        "African Baobab (Mbuyu)",
        "Yellow Fever Tree (Olerai)",
        "Mugumo Sacred Fig",
        "African Blackwood (Mpingo)",
        "Nandi Flame (African Tulip)",
        "Sycamore Fig (Mukuyu)",
        "Whistling Thorn (Eluai)",
        "African Wild Olive (Mutamaiyu)",
        "Meru Oak (East African Teak)",
        "Sausage Tree (Muratina)",
        "Coconut Palm (Mnazi)",
        "Date Palm (Desert Oasis Palm)",
        "Coast Redwood (Giant Conifer)",
        "Red Mangrove (Mkoko)",
        "Whistling Pine (Mvinje / Casuarina)",
        "Doum Palm (Turkana Bifurcated Fan Palm)",
        "Desert Rose (Adenium Obesum Caudex)",
        "Candelabra Euphorbia (Succulent Arms)",
        "Banana Plant (Musa Paddle Fronds)",
        "Mango Tree (Dense Dome Canopy)",
        "Papaya Tree (Carica Crown Column)",
        "Jacaranda (Blue Mist)",
        "Flamboyant (Royal Poinciana)",
        "Golden Poui (Araguaney)",
        "Southern Magnolia",
        "Crape Myrtle",
        "Golden Shower Tree",
        "Frangipani (Plumeria)",
        "Crimson Bottlebrush",
        "Cape Chestnut (Muraru)",
        "Italian Cypress",
        "Japanese Maple (Momiji)",
        "Standard Bougainvillea",
        "Flowering Dogwood",
        "Avocado Tree (Parachichi)",
        "Sweet Orange (Mchungwa)",
        "Meyer Lemon Tree (Mlimau)",
        "Macadamia Nut Tree",
        "Jackfruit Tree (Mfenesi)",
        "Guava Tree (Mpera)",
        "Passion Fruit Vine (Matunda)",
        "Pomegranate (Komamanga)",
        "Orchard Apple Tree",
        "European Pear Tree",
        "Peach Tree",
        "Cocoa / Cacao Tree",
        "Breadfruit Tree (Mshelisheli)",
        "Lychee Tree",
        "Cashew Tree (Mkorosho)"
    };
}

std::vector<TreeSpeciesId> TreeTypeRegistry::getAvailableTreeTypeIds() {
    return {
        TreeSpeciesId::SCOTS_PINE,
        TreeSpeciesId::ENGLISH_OAK,
        TreeSpeciesId::NORWAY_SPRUCE,
        TreeSpeciesId::GIANT_SEQUOIA,
        TreeSpeciesId::SILVER_BIRCH,
        TreeSpeciesId::EUROPEAN_BEECH,
        TreeSpeciesId::SUGAR_MAPLE,
        TreeSpeciesId::WEEPING_WILLOW,
        TreeSpeciesId::GINKGO_BILOBA,
        TreeSpeciesId::SAVANNA_ACACIA,
        TreeSpeciesId::MEDITERRANEAN_OLIVE,
        TreeSpeciesId::SWEET_CHESTNUT,
        TreeSpeciesId::EUROPEAN_LARCH,
        TreeSpeciesId::JAPANESE_CHERRY,
        TreeSpeciesId::ATLAS_CEDAR,
        TreeSpeciesId::RED_MAPLE,
        TreeSpeciesId::AFRICAN_BAOBAB,
        TreeSpeciesId::YELLOW_FEVER_TREE,
        TreeSpeciesId::MUGUMO_SACRED_FIG,
        TreeSpeciesId::AFRICAN_BLACKWOOD,
        TreeSpeciesId::NANDI_FLAME,
        TreeSpeciesId::SYCAMORE_FIG,
        TreeSpeciesId::WHISTLING_THORN,
        TreeSpeciesId::AFRICAN_WILD_OLIVE,
        TreeSpeciesId::MERU_OAK,
        TreeSpeciesId::SAUSAGE_TREE,
        TreeSpeciesId::COCONUT_PALM,
        TreeSpeciesId::DATE_PALM,
        TreeSpeciesId::GIANT_REDWOOD,
        TreeSpeciesId::RED_MANGROVE,
        TreeSpeciesId::BEACH_CASUARINA,
        TreeSpeciesId::DOUM_PALM,
        TreeSpeciesId::DESERT_ROSE,
        TreeSpeciesId::CANDELABRA_EUPHORBIA,
        TreeSpeciesId::BANANA_TREE,
        TreeSpeciesId::MANGO_TREE,
        TreeSpeciesId::PAPAYA_TREE,
        TreeSpeciesId::JACARANDA,
        TreeSpeciesId::FLAMBOYANT_POINCIANA,
        TreeSpeciesId::YELLOW_TRUMPET_TREE,
        TreeSpeciesId::SOUTHERN_MAGNOLIA,
        TreeSpeciesId::CREPE_MYRTLE,
        TreeSpeciesId::GOLDEN_SHOWER_CASSIA,
        TreeSpeciesId::FRANGIPANI_PLUMERIA,
        TreeSpeciesId::CRIMSON_BOTTLEBRUSH,
        TreeSpeciesId::CAPE_CHESTNUT,
        TreeSpeciesId::ITALIAN_CYPRESS,
        TreeSpeciesId::JAPANESE_MAPLE,
        TreeSpeciesId::BOUGAINVILLEA_STANDARD,
        TreeSpeciesId::FLOWERING_DOGWOOD,
        TreeSpeciesId::AVOCADO_TREE,
        TreeSpeciesId::SWEET_ORANGE,
        TreeSpeciesId::LEMON_TREE,
        TreeSpeciesId::MACADAMIA_TREE,
        TreeSpeciesId::JACKFRUIT_TREE,
        TreeSpeciesId::GUAVA_TREE,
        TreeSpeciesId::PASSION_FRUIT_STANDARD,
        TreeSpeciesId::POMEGRANATE_TREE,
        TreeSpeciesId::HONEYCRISP_APPLE,
        TreeSpeciesId::SWEET_PEAR,
        TreeSpeciesId::PEACH_TREE,
        TreeSpeciesId::CACAO_TREE,
        TreeSpeciesId::BREADFRUIT_TREE,
        TreeSpeciesId::LYCHEE_TREE,
        TreeSpeciesId::CASHEW_TREE
    };
}

} // namespace vegetation
