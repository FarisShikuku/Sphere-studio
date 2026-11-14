#pragma once
#include <cstdint>
#include <string>

namespace sphere::cargo {

enum class CargoCategory : uint32_t {
    BAGGED_GOODS = 0,
    BOXES_CRATES,
    LIQUID_BULK,
    RAW_MATERIALS,
    CONTAINERS_BOTTLES,
    SPECIALTY_INDUSTRIAL,
    PERISHABLE_GOODS,
    BUILDING_MATERIALS,
    COUNT
};

enum class CargoID : uint32_t {
    // Bagged Goods
    SUGAR_BAG_50KG = 0,
    MAIZE_100KG,
    CEMENT_50KG,
    FERTILIZER_25KG,
    RICE_25KG,
    COFFEE_BEAN_60KG,
    TEA_LEAF_50KG,

    // Boxes & Crates
    CARDBOARD_BOX,
    WOODEN_CRATE,
    PLASTIC_CRATE,
    PALLET,
    MILK_CRATE,

    // Liquid Bulk
    WATER_TANKER,
    OIL_DRUM_200L,
    JERRICAN_20L,
    MILK_CHURN,
    GAS_CYLINDER,

    // Raw Materials
    WOOD_LOG_OAK,
    TIMBER_PLANK,
    STEEL_REBAR,
    COAL_PILE,
    GRAVEL_PILE,

    // Containers & Bottles
    GLASS_BOTTLE_BEER,
    PLASTIC_BOTTLE_SODA,
    PLASTIC_BOTTLE_WATER,
    JAR_GLASS,
    TIN_CAN,

    // Specialty Industrial
    CEMENT_BAG_50KG,
    STEEL_PIPE_6M,
    BRICK_PALLET,
    TYRE_STACK,

    // Perishable Goods
    PRODUCE_CRATE_TOMATO,
    PRODUCE_BAG_POTATO,
    FISH_CRATE,
    MEAT_BOX,

    // Chopped Wood & Forestry Variants (Raw Materials)
    CHOPPED_FIREWOOD_SPLIT,
    HALF_ROUND_FIREWOOD,
    STACKED_CORDWOOD_RICK,
    WOODEN_RAILWAY_SLEEPER,
    CHOPPED_KINDLING_BUNDLE,
    TIMBER_STUMP_BLOCKS,

    // Building Materials (Structural Beams, Sand, Architectural Glass, Concrete & Roofing)
    STEEL_IBEAM_6M,
    TIMBER_GLULAM_BEAM,
    SILICA_SAND_HEAP,
    SILICA_SAND_FIBC_1T,
    FLOAT_GLASS_STILLAGE,
    HOLLOW_CONCRETE_CMU,
    PRECAST_CONCRETE_SLAB,
    GYPSUM_DRYWALL_STACK,
    CORRUGATED_ROOF_SHEETS,
    TERRA_ROOF_TILES_PALLET,

    COUNT
};

const char* GetCargoName(CargoID id);
const char* GetCargoCategoryName(CargoCategory cat);
const char* GetCargoCategory(CargoID id);
CargoCategory GetCargoCategoryEnum(CargoID id);

} // namespace sphere::cargo
