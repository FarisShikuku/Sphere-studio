#include "BoxesCratesVariants.h"

namespace sphere::cargo {

static const std::vector<BoxVariantConfig> kBoxVariants = {
    {
        CargoID::CARDBOARD_BOX,
        "CARDBOARD_BOX",
        "Corrugated Shipping Carton (Standard)",
        12.5f,
        0.60f, 0.40f, 0.35f,
        BoxConstruction::CORRUGATED_CARDBOARD,
        Vec4(0.72f, 0.58f, 0.42f, 1.0f),
        Vec4(0.15f, 0.15f, 0.15f, 1.0f),
        "FRAGILE - THIS WAY UP",
        0.82f,
        0.0f,
        0.008f
    },
    {
        CargoID::WOODEN_CRATE,
        "WOODEN_CRATE",
        "Industrial Slatted Pine Crate (Reinforced)",
        45.0f,
        1.00f, 0.80f, 0.65f,
        BoxConstruction::SLATTED_TIMBER_WOOD,
        Vec4(0.78f, 0.64f, 0.46f, 1.0f),
        Vec4(0.20f, 0.20f, 0.20f, 1.0f),
        "HEAVY MACHINERY PART",
        0.75f,
        0.09f,
        0.0f
    },
    {
        CargoID::PLASTIC_CRATE,
        "PLASTIC_CRATE",
        "Stackable Vented HDPE Field Crate",
        18.0f,
        0.50f, 0.35f, 0.30f,
        BoxConstruction::VENTED_MOLDED_PLASTIC,
        Vec4(0.12f, 0.42f, 0.80f, 1.0f),
        Vec4(0.95f, 0.95f, 0.95f, 1.0f),
        "AGRI-LOGISTICS 40L",
        0.35f,
        0.04f,
        0.0f
    },
    {
        CargoID::PALLET,
        "PALLET",
        "EUR-1 Four-Way Wooden Exchange Pallet",
        25.0f,
        1.20f, 0.80f, 0.144f,
        BoxConstruction::WOODEN_EXCHANGE_PALLET,
        Vec4(0.70f, 0.58f, 0.42f, 1.0f),
        Vec4(0.10f, 0.10f, 0.10f, 1.0f),
        "EPAL EUR 1200x800",
        0.88f,
        0.145f,
        0.0f
    },
    {
        CargoID::MILK_CRATE,
        "MILK_CRATE",
        "Heavy-Duty Solid Wall Dairy Milk Crate",
        16.0f,
        0.38f, 0.38f, 0.28f,
        BoxConstruction::REINFORCED_MILK_CRATE,
        Vec4(0.85f, 0.20f, 0.15f, 1.0f),
        Vec4(0.95f, 0.95f, 0.95f, 1.0f),
        "DAIRY FRESH CRATE",
        0.40f,
        0.035f,
        0.0f
    }
};

const std::vector<BoxVariantConfig>& BoxesCratesVariants::GetAllVariants() {
    return kBoxVariants;
}

const BoxVariantConfig* BoxesCratesVariants::FindVariant(CargoID id) {
    for (const auto& v : kBoxVariants) {
        if (v.id == id) return &v;
    }
    return nullptr;
}

CargoDef BoxesCratesVariants::CreateCargoDef(const BoxVariantConfig& cfg) {
    CargoDef def;
    def.id = cfg.id;
    def.name = cfg.displayName;
    def.category = "Boxes & Crates";
    def.packingType = (cfg.id == CargoID::PALLET) ? CargoPackingType::PALLETIZED : CargoPackingType::CRATE;
    def.weight_kg = cfg.nominalWeightKg;
    def.dimensions = Vec3(cfg.lengthMeters, cfg.widthMeters, cfg.heightMeters);
    def.volume_m3 = cfg.lengthMeters * cfg.widthMeters * cfg.heightMeters;
    def.density_kg_m3 = def.weight_kg / std::max(0.001f, def.volume_m3);
    def.max_stack_load_kg = (cfg.id == CargoID::PALLET) ? 4000.0f : 850.0f;
    def.is_liquid_or_bulk = false;
    def.is_perishable = false;

    def.albedo_format = VK_FORMAT_BC7_SRGB_BLOCK;
    def.normal_format = VK_FORMAT_BC5_UNORM_BLOCK;
    def.orm_format = VK_FORMAT_BC7_UNORM_BLOCK;
    def.texture_width = 1024;
    def.texture_height = 1024;

    def.base_color = cfg.primaryColor;
    def.roughness = cfg.roughness;
    def.metallic = (cfg.construction == BoxConstruction::SLATTED_TIMBER_WOOD) ? 0.05f : 0.0f; // nail heads
    def.opacity_bias = 1.0f;

    def.label_text = cfg.labelText;
    def.brand_id = cfg.variantCode;
    def.label_color = cfg.secondaryColor;

    def.wrinkle_or_corrugation = (cfg.construction == BoxConstruction::CORRUGATED_CARDBOARD) ? 0.5f : 0.15f;
    return def;
}

} // namespace sphere::cargo
