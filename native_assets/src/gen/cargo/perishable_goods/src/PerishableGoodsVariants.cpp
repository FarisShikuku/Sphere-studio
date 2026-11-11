#include "PerishableGoodsVariants.h"

namespace sphere::cargo {

static const std::vector<PerishableVariantConfig> kPerishableVariants = {
    {
        CargoID::PRODUCE_CRATE_TOMATO,
        "PRODUCE_CRATE_TOMATO",
        "Vine-Ripened Harvest Tomatoes in Vented Field Crate",
        14.0f,
        0.50f, 0.35f, 0.25f,
        PerishableType::HARVEST_PRODUCE_CRATE,
        Vec4(0.88f, 0.18f, 0.12f, 1.0f), // Tomato red
        Vec4(0.18f, 0.55f, 0.22f, 1.0f), // Green calyx stems
        0.25f, // Smooth glossy tomato skin
        0.65f, // Wet morning harvest shine
        12.0f  // +12 C cool chain
    },
    {
        CargoID::PRODUCE_BAG_POTATO,
        "PRODUCE_BAG_POTATO",
        "Washed Russet Potatoes in Breathable Mesh Bag 25kg",
        25.0f,
        0.65f, 0.42f, 0.22f,
        PerishableType::VENTILATED_ROOT_BAG,
        Vec4(0.72f, 0.58f, 0.42f, 1.0f), // Earthy potato skin
        Vec4(0.85f, 0.20f, 0.15f, 1.0f), // Red mesh netting
        0.78f,
        0.15f,
        8.0f   // +8 C storage
    },
    {
        CargoID::FISH_CRATE,
        "FISH_CRATE",
        "Fresh Wild-Caught Salmon on Crushed Ice Crate",
        22.0f,
        0.60f, 0.40f, 0.20f,
        PerishableType::ICED_SEAFOOD_CRATE,
        Vec4(0.92f, 0.45f, 0.38f, 1.0f), // Salmon flesh pink
        Vec4(0.94f, 0.96f, 0.98f, 1.0f), // Crushed flake ice
        0.18f,
        0.95f, // Wet melting ice specular
        0.0f   // 0 C ice preservation
    },
    {
        CargoID::MEAT_BOX,
        "MEAT_BOX",
        "Refrigerated Insulated Vacuum-Packed Beef Primal Carton",
        28.0f,
        0.55f, 0.38f, 0.28f,
        PerishableType::INSULATED_MEAT_BOX,
        Vec4(0.68f, 0.15f, 0.18f, 1.0f), // Deep muscle crimson
        Vec4(0.98f, 0.98f, 0.98f, 1.0f), // Insulated wax carton
        0.45f,
        0.40f,
        -1.5f  // -1.5 C deep chill
    }
};

const std::vector<PerishableVariantConfig>& PerishableGoodsVariants::GetAllVariants() {
    return kPerishableVariants;
}

const PerishableVariantConfig* PerishableGoodsVariants::FindVariant(CargoID id) {
    for (const auto& v : kPerishableVariants) {
        if (v.id == id) return &v;
    }
    return nullptr;
}

CargoDef PerishableGoodsVariants::CreateCargoDef(const PerishableVariantConfig& cfg) {
    CargoDef def;
    def.id = cfg.id;
    def.name = cfg.displayName;
    def.category = "Perishable Goods";
    def.packingType = CargoPackingType::PERISHABLE;
    def.weight_kg = cfg.nominalWeightKg;
    def.dimensions = Vec3(cfg.lengthMeters, cfg.widthMeters, cfg.heightMeters);
    def.volume_m3 = cfg.lengthMeters * cfg.widthMeters * cfg.heightMeters;
    def.density_kg_m3 = def.weight_kg / std::max(0.001f, def.volume_m3);
    def.max_stack_load_kg = 280.0f;
    def.is_liquid_or_bulk = false;
    def.is_perishable = true;

    def.albedo_format = VK_FORMAT_BC7_SRGB_BLOCK;
    def.normal_format = VK_FORMAT_BC5_UNORM_BLOCK;
    def.orm_format = VK_FORMAT_BC7_UNORM_BLOCK;
    def.texture_width = 1024;
    def.texture_height = 1024;

    def.base_color = cfg.primaryColor;
    def.roughness = cfg.roughness;
    def.metallic = 0.0f;
    def.opacity_bias = 1.0f;

    def.label_text = cfg.displayName;
    def.brand_id = cfg.variantCode;
    def.label_color = cfg.secondaryColor;

    def.wrinkle_or_corrugation = cfg.wetnessSpecular;
    return def;
}

} // namespace sphere::cargo
