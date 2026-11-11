#include "CargoID.h"

namespace sphere::cargo {

const char* GetCargoCategoryName(CargoCategory cat) {
    switch (cat) {
        case CargoCategory::BAGGED_GOODS: return "Bagged Goods";
        case CargoCategory::BOXES_CRATES: return "Boxes & Crates";
        case CargoCategory::LIQUID_BULK: return "Liquid Bulk";
        case CargoCategory::RAW_MATERIALS: return "Raw Materials";
        case CargoCategory::CONTAINERS_BOTTLES: return "Containers & Bottles";
        case CargoCategory::SPECIALTY_INDUSTRIAL: return "Specialty Industrial";
        case CargoCategory::PERISHABLE_GOODS: return "Perishable Goods";
        default: return "Unknown";
    }
}

CargoCategory GetCargoCategoryEnum(CargoID id) {
    if (id <= CargoID::TEA_LEAF_50KG) return CargoCategory::BAGGED_GOODS;
    if (id <= CargoID::MILK_CRATE) return CargoCategory::BOXES_CRATES;
    if (id <= CargoID::GAS_CYLINDER) return CargoCategory::LIQUID_BULK;
    if (id <= CargoID::GRAVEL_PILE) return CargoCategory::RAW_MATERIALS;
    if (id <= CargoID::TIN_CAN) return CargoCategory::CONTAINERS_BOTTLES;
    if (id <= CargoID::TYRE_STACK) return CargoCategory::SPECIALTY_INDUSTRIAL;
    return CargoCategory::PERISHABLE_GOODS;
}

const char* GetCargoCategory(CargoID id) {
    return GetCargoCategoryName(GetCargoCategoryEnum(id));
}

const char* GetCargoName(CargoID id) {
    switch (id) {
        case CargoID::SUGAR_BAG_50KG: return "Sugar Bag 50kg (Woven Polypropylene)";
        case CargoID::MAIZE_100KG: return "Maize Sack 100kg (Bulk Polypropylene)";
        case CargoID::CEMENT_50KG: return "Cement Bag 50kg (Multiwall Kraft Paper)";
        case CargoID::FERTILIZER_25KG: return "NPK Fertilizer 25kg (Laminated PE)";
        case CargoID::RICE_25KG: return "Milled Rice Sack 25kg (Tight Weave)";
        case CargoID::COFFEE_BEAN_60KG: return "Raw Coffee Bean 60kg (Traditional Jute)";
        case CargoID::TEA_LEAF_50KG: return "Processed Tea Leaf 50kg (Foil-Lined Kraft)";

        case CargoID::CARDBOARD_BOX: return "Corrugated Cardboard Box (Shipping)";
        case CargoID::WOODEN_CRATE: return "Slatted Wooden Crate (Reinforced)";
        case CargoID::PLASTIC_CRATE: return "Vented HDPE Produce Crate";
        case CargoID::PALLET: return "EUR-1 Wooden Exchange Pallet (1200x800mm)";
        case CargoID::MILK_CRATE: return "Heavy-Duty Stackable Milk Crate";

        case CargoID::WATER_TANKER: return "Bulk Liquid Tanker Module";
        case CargoID::OIL_DRUM_200L: return "Steel Chemical Drum 200L (Tight-Head)";
        case CargoID::JERRICAN_20L: return "Polyethylene Jerrican 20L";
        case CargoID::MILK_CHURN: return "Stainless Steel Dairy Churn 40L";
        case CargoID::GAS_CYLINDER: return "Pressurized LPG Cylinder 15kg";

        case CargoID::WOOD_LOG_OAK: return "Raw Oak Timber Log (Debarked End)";
        case CargoID::TIMBER_PLANK: return "Milled Construction Timber Plank";
        case CargoID::STEEL_REBAR: return "Deformed High-Yield Rebar Bundle";
        case CargoID::COAL_PILE: return "Anthracite Bulk Coal Mound";
        case CargoID::GRAVEL_PILE: return "Crushed Stone Aggregate Gravel Pile";

        case CargoID::GLASS_BOTTLE_BEER: return "Amber Glass Beer Bottle (330ml Crown)";
        case CargoID::PLASTIC_BOTTLE_SODA: return "PET Carbonated Soda Bottle (2L)";
        case CargoID::PLASTIC_BOTTLE_WATER: return "Clear PET Mineral Water Bottle (500ml)";
        case CargoID::JAR_GLASS: return "Preserve Glass Jar with Lug Cap (500ml)";
        case CargoID::TIN_CAN: return "Sanitary Steel Tin Can (400g Food)";

        case CargoID::CEMENT_BAG_50KG: return "Industrial Portland Cement 50kg";
        case CargoID::STEEL_PIPE_6M: return "Structural Steel Pipe Bundle 6m";
        case CargoID::BRICK_PALLET: return "Fired Clay Brick Pallet (Banded)";
        case CargoID::TYRE_STACK: return "Heavy Commercial Truck Tyre Stack";

        case CargoID::PRODUCE_CRATE_TOMATO: return "Fresh Vine Tomato Harvest Crate";
        case CargoID::PRODUCE_BAG_POTATO: return "Ventilated Mesh Potato Bag 25kg";
        case CargoID::FISH_CRATE: return "Iced Fresh Catch Seafood Crate";
        case CargoID::MEAT_BOX: return "Refrigerated Insulated Meat Carton";
        default: return "Custom Cargo Commodity";
    }
}

} // namespace sphere::cargo
