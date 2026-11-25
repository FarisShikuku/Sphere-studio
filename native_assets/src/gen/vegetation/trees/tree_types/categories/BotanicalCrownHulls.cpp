// ============================================================================
// BotanicalCrownHulls.cpp
// PlantGL Asymmetric Crown Hull Parametrization per Botanical Morphotype
// ============================================================================

#include "TreeCategoryBuilders.h"

namespace vegetation {
namespace categories {

CrownControlParams computeSpeciesCrownHull(const TreeBuildContext& ctx) {
    CrownControlParams p;
    p.leafCount = ctx.leafCount;
    const float H = ctx.treeHeight;

    switch (ctx.speciesId) {
        case TreeSpeciesId::CANDELABRA_EUPHORBIA:
            p.trunkHeight = H * 0.32f;
            p.crownHeight = H * 0.68f;
            p.rxPos = H * 0.36f; p.rxNeg = H * 0.34f;
            p.ryPos = H * 0.36f; p.ryNeg = H * 0.34f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.70f;
            p.hzXNeg = p.hzXPos; p.hzYPos = p.hzXPos; p.hzYNeg = p.hzXPos;
            p.shapeTop = 2.40f; p.shapeBot = 1.30f;
            break;

        case TreeSpeciesId::DATE_PALM:
        case TreeSpeciesId::COCONUT_PALM:
        case TreeSpeciesId::DOUM_PALM:
        case TreeSpeciesId::BANANA_TREE:
        case TreeSpeciesId::PAPAYA_TREE:
            p.trunkHeight = H * (ctx.speciesId == TreeSpeciesId::BANANA_TREE ? 0.58f : 0.70f);
            p.crownHeight = H - p.trunkHeight;
            p.rxPos = H * 0.28f; p.rxNeg = H * 0.26f;
            p.ryPos = H * 0.28f; p.ryNeg = H * 0.26f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.35f;
            p.hzXNeg = p.hzXPos; p.hzYPos = p.hzXPos; p.hzYNeg = p.hzXPos;
            p.shapeTop = 1.30f; p.shapeBot = 2.40f;
            break;

        case TreeSpeciesId::DESERT_ROSE:
        case TreeSpeciesId::FRANGIPANI_PLUMERIA:
            p.trunkHeight = H * 0.38f;
            p.crownHeight = H * 0.62f;
            p.rxPos = H * 0.42f; p.rxNeg = H * 0.40f;
            p.ryPos = H * 0.42f; p.ryNeg = H * 0.40f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.45f;
            p.hzXNeg = p.hzXPos; p.hzYPos = p.hzXPos; p.hzYNeg = p.hzXPos;
            p.shapeTop = 1.60f; p.shapeBot = 1.80f;
            break;

        case TreeSpeciesId::AFRICAN_BAOBAB:
            p.trunkHeight = H * 0.52f;
            p.crownHeight = H * 0.48f;
            p.rxPos = H * 0.50f; p.rxNeg = H * 0.48f;
            p.ryPos = H * 0.50f; p.ryNeg = H * 0.48f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.30f;
            p.hzXNeg = p.hzXPos; p.hzYPos = p.hzXPos; p.hzYNeg = p.hzXPos;
            p.shapeTop = 1.35f; p.shapeBot = 2.20f;
            break;

        case TreeSpeciesId::SAVANNA_ACACIA:
        case TreeSpeciesId::YELLOW_FEVER_TREE:
        case TreeSpeciesId::WHISTLING_THORN:
        case TreeSpeciesId::AFRICAN_BLACKWOOD:
        case TreeSpeciesId::FLAMBOYANT_POINCIANA:
            p.trunkHeight = H * 0.46f;
            p.crownHeight = H * 0.54f;
            p.rxPos = H * 0.58f; p.rxNeg = H * 0.55f;
            p.ryPos = H * 0.58f; p.ryNeg = H * 0.55f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.88f;
            p.hzXNeg = p.hzXPos; p.hzYPos = p.hzXPos; p.hzYNeg = p.hzXPos;
            p.shapeTop = 4.20f; p.shapeBot = 1.15f;
            break;

        case TreeSpeciesId::ITALIAN_CYPRESS:
            p.trunkHeight = H * 0.14f;
            p.crownHeight = H * 0.86f;
            p.rxPos = H * 0.10f; p.rxNeg = H * 0.09f;
            p.ryPos = H * 0.10f; p.ryNeg = H * 0.09f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.30f;
            p.hzXNeg = p.hzXPos; p.hzYPos = p.hzXPos; p.hzYNeg = p.hzXPos;
            p.shapeTop = 0.72f; p.shapeBot = 1.25f;
            break;

        case TreeSpeciesId::NORWAY_SPRUCE:
        case TreeSpeciesId::SCOTS_PINE:
        case TreeSpeciesId::GIANT_REDWOOD:
        case TreeSpeciesId::GIANT_SEQUOIA:
        case TreeSpeciesId::EUROPEAN_LARCH:
        case TreeSpeciesId::ATLAS_CEDAR:
        case TreeSpeciesId::BEACH_CASUARINA: {
            bool tallForestConifer = (ctx.speciesId == TreeSpeciesId::GIANT_REDWOOD ||
                                      ctx.speciesId == TreeSpeciesId::GIANT_SEQUOIA ||
                                      ctx.speciesId == TreeSpeciesId::SCOTS_PINE ||
                                      ctx.speciesId == TreeSpeciesId::BEACH_CASUARINA);
            float boleFrac = tallForestConifer ? 0.38f : 0.24f;
            p.trunkHeight = H * boleFrac;
            p.crownHeight = H * (1.0f - boleFrac);
            p.rxPos = H * 0.24f; p.rxNeg = H * 0.22f;
            p.ryPos = H * 0.23f; p.ryNeg = H * 0.21f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.25f;
            p.hzXNeg = p.hzXPos; p.hzYPos = p.hzXPos; p.hzYNeg = p.hzXPos;
            p.shapeTop = 0.85f; p.shapeBot = 1.10f;
            break;
        }

        case TreeSpeciesId::PASSION_FRUIT_STANDARD:
            p.trunkHeight = H * 0.05f;
            p.crownHeight = H * 0.95f;
            p.rxPos = H * 0.56f; p.rxNeg = H * 0.54f;
            p.ryPos = H * 0.56f; p.ryNeg = H * 0.54f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.45f;
            p.hzXNeg = p.hzXPos; p.hzYPos = p.hzXPos; p.hzYNeg = p.hzXPos;
            p.shapeTop = 1.65f; p.shapeBot = 1.15f;
            break;

        case TreeSpeciesId::CREPE_MYRTLE:
        case TreeSpeciesId::JAPANESE_MAPLE:
        case TreeSpeciesId::GUAVA_TREE:
        case TreeSpeciesId::POMEGRANATE_TREE:
            p.trunkHeight = H * 0.24f;
            p.crownHeight = H * 0.76f;
            p.rxPos = H * 0.44f; p.rxNeg = H * 0.42f;
            p.ryPos = H * 0.44f; p.ryNeg = H * 0.42f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.56f;
            p.hzXNeg = p.hzXPos; p.hzYPos = p.hzXPos; p.hzYNeg = p.hzXPos;
            p.shapeTop = 1.95f; p.shapeBot = 1.45f;
            break;

        case TreeSpeciesId::WEEPING_WILLOW:
            p.trunkHeight = H * 0.36f;
            p.crownHeight = H * 0.64f;
            p.rxPos = H * 0.42f; p.rxNeg = H * 0.40f;
            p.ryPos = H * 0.42f; p.ryNeg = H * 0.40f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.55f;
            p.hzXNeg = p.hzXPos; p.hzYPos = p.hzXPos; p.hzYNeg = p.hzXPos;
            p.shapeTop = 1.50f; p.shapeBot = 0.80f;
            break;

        case TreeSpeciesId::RED_MANGROVE:
            p.trunkHeight = H * 0.28f;
            p.crownHeight = H * 0.72f;
            p.rxPos = H * 0.40f; p.rxNeg = H * 0.38f;
            p.ryPos = H * 0.40f; p.ryNeg = H * 0.38f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.40f;
            p.hzXNeg = p.hzXPos; p.hzYPos = p.hzXPos; p.hzYNeg = p.hzXPos;
            p.shapeTop = 1.90f; p.shapeBot = 1.80f;
            break;

        case TreeSpeciesId::MUGUMO_SACRED_FIG:
        case TreeSpeciesId::SYCAMORE_FIG:
            p.trunkHeight = H * 0.44f;
            p.crownHeight = H * 0.56f;
            p.rxPos = H * 0.48f; p.rxNeg = H * 0.45f;
            p.ryPos = H * 0.47f; p.ryNeg = H * 0.44f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.48f;
            p.hzXNeg = p.hzXPos; p.hzYPos = p.hzXPos; p.hzYNeg = p.hzXPos;
            p.shapeTop = 2.35f; p.shapeBot = 1.50f;
            break;

        case TreeSpeciesId::SWEET_PEAR:
            p.trunkHeight = H * 0.28f;
            p.crownHeight = H * 0.72f;
            p.rxPos = H * 0.26f; p.rxNeg = H * 0.25f;
            p.ryPos = H * 0.26f; p.ryNeg = H * 0.25f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.30f;
            p.hzXNeg = p.hzXPos; p.hzYPos = p.hzXPos; p.hzYNeg = p.hzXPos;
            p.shapeTop = 1.25f; p.shapeBot = 1.65f;
            break;

        case TreeSpeciesId::HONEYCRISP_APPLE:
        case TreeSpeciesId::PEACH_TREE:
        case TreeSpeciesId::CASHEW_TREE:
        case TreeSpeciesId::SWEET_ORANGE:
        case TreeSpeciesId::LEMON_TREE:
        case TreeSpeciesId::MEDITERRANEAN_OLIVE:
        case TreeSpeciesId::FLOWERING_DOGWOOD:
        case TreeSpeciesId::CRIMSON_BOTTLEBRUSH:
            p.trunkHeight = H * 0.22f;
            p.crownHeight = H * 0.78f;
            p.rxPos = H * 0.48f; p.rxNeg = H * 0.46f;
            p.ryPos = H * 0.48f; p.ryNeg = H * 0.46f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.46f;
            p.hzXNeg = p.hzXPos; p.hzYPos = p.hzXPos; p.hzYNeg = p.hzXPos;
            p.shapeTop = 2.20f; p.shapeBot = 1.48f;
            break;

        case TreeSpeciesId::CACAO_TREE:
            p.trunkHeight = H * 0.30f;
            p.crownHeight = H * 0.70f;
            p.rxPos = H * 0.44f; p.rxNeg = H * 0.42f;
            p.ryPos = H * 0.44f; p.ryNeg = H * 0.42f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.48f;
            p.hzXNeg = p.hzXPos; p.hzYPos = p.hzXPos; p.hzYNeg = p.hzXPos;
            p.shapeTop = 2.15f; p.shapeBot = 1.55f;
            break;

        case TreeSpeciesId::MERU_OAK:
        case TreeSpeciesId::NANDI_FLAME:
        case TreeSpeciesId::BREADFRUIT_TREE:
        case TreeSpeciesId::JACKFRUIT_TREE:
        case TreeSpeciesId::SOUTHERN_MAGNOLIA:
        case TreeSpeciesId::CAPE_CHESTNUT:
        case TreeSpeciesId::EUROPEAN_BEECH:
        case TreeSpeciesId::GINKGO_BILOBA:
        case TreeSpeciesId::SWEET_CHESTNUT:
            // Tall tropical rainforest & mature forest canopy trees with elevated clear trunk bole
            p.trunkHeight = H * 0.46f;
            p.crownHeight = H * 0.54f;
            p.rxPos = H * 0.38f; p.rxNeg = H * 0.35f;
            p.ryPos = H * 0.37f; p.ryNeg = H * 0.34f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.44f;
            p.hzXNeg = p.trunkHeight + p.crownHeight * 0.45f;
            p.hzYPos = p.trunkHeight + p.crownHeight * 0.42f;
            p.hzYNeg = p.trunkHeight + p.crownHeight * 0.46f;
            p.shapeTop = 2.05f; p.shapeBot = 1.70f;
            break;

        default:
            // Mature broadleaf & tropical orchard canopy trees (Avocado, Mango, Oak, Maple, Lychee, etc.)
            p.trunkHeight = H * 0.38f;
            p.crownHeight = H * 0.62f;
            p.rxPos = H * 0.38f; p.rxNeg = H * 0.35f;
            p.ryPos = H * 0.37f; p.ryNeg = H * 0.34f;
            p.hzXPos = p.trunkHeight + p.crownHeight * 0.44f;
            p.hzXNeg = p.trunkHeight + p.crownHeight * 0.45f;
            p.hzYPos = p.trunkHeight + p.crownHeight * 0.42f;
            p.hzYNeg = p.trunkHeight + p.crownHeight * 0.46f;
            p.shapeTop = 2.05f; p.shapeBot = 1.70f;
            break;
    }

    return p;
}

} // namespace categories
} // namespace vegetation
