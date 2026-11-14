#include "RawMaterialsVariants.h"

namespace sphere::cargo {

static const std::vector<RawVariantConfig> kRawVariants = {
    {
        CargoID::WOOD_LOG_OAK,
        "WOOD_LOG_OAK",
        "Raw Hardwood Oak Tree Log (Bark & Annual Rings)",
        420.0f,
        2.40f, 0.45f, 0.45f,
        RawMaterialForm::CYLINDRICAL_TIMBER_LOG,
        Vec4(0.48f, 0.38f, 0.28f, 1.0f),
        Vec4(0.82f, 0.70f, 0.52f, 1.0f), // Cut end sapwood
        0.88f,
        0.0f,
        0.65f
    },
    {
        CargoID::TIMBER_PLANK,
        "TIMBER_PLANK",
        "Kiln-Dried Dimensional Construction Timber (Pack of 12)",
        210.0f,
        3.00f, 0.60f, 0.40f,
        RawMaterialForm::MILLED_RECTANGULAR_LUMBER,
        Vec4(0.82f, 0.68f, 0.48f, 1.0f),
        Vec4(0.20f, 0.20f, 0.20f, 1.0f),
        0.72f,
        0.0f,
        0.20f
    },
    {
        CargoID::STEEL_REBAR,
        "STEEL_REBAR",
        "High-Yield Deformed Carbon Steel Rebar Bundle (Banded)",
        850.0f,
        3.50f, 0.35f, 0.35f,
        RawMaterialForm::DEFORMED_STEEL_REBAR_BUNDLE,
        Vec4(0.35f, 0.38f, 0.42f, 1.0f),
        Vec4(0.65f, 0.35f, 0.15f, 1.0f), // Rust dusting
        0.58f,
        0.92f,
        0.45f
    },
    {
        CargoID::COAL_PILE,
        "COAL_PILE",
        "Anthracite High-Carbon Bulk Coal Mound",
        1200.0f,
        1.80f, 1.80f, 0.75f,
        RawMaterialForm::BULK_COAL_PILE,
        Vec4(0.12f, 0.12f, 0.14f, 1.0f),
        Vec4(0.05f, 0.05f, 0.06f, 1.0f),
        0.65f,
        0.18f,
        0.85f
    },
    {
        CargoID::GRAVEL_PILE,
        "GRAVEL_PILE",
        "Crushed Granite Stone Aggregate Mound (20mm)",
        1450.0f,
        1.80f, 1.80f, 0.70f,
        RawMaterialForm::AGGREGATE_GRAVEL_MOUND,
        Vec4(0.65f, 0.65f, 0.68f, 1.0f),
        Vec4(0.45f, 0.45f, 0.48f, 1.0f),
        0.92f,
        0.02f,
        0.90f
    },
    {
        CargoID::CHOPPED_FIREWOOD_SPLIT,
        "CHOPPED_FIREWOOD_SPLIT",
        "Quarter-Split Seasoned Oak Firewood Wedges",
        42.0f,
        0.48f, 0.36f, 0.32f,
        RawMaterialForm::QUARTER_SPLIT_WEDGE_FIREWOOD,
        Vec4(0.42f, 0.30f, 0.20f, 1.0f),
        Vec4(0.84f, 0.68f, 0.44f, 1.0f),
        0.86f,
        0.0f,
        0.72f
    },
    {
        CargoID::HALF_ROUND_FIREWOOD,
        "HALF_ROUND_FIREWOOD",
        "Half-Split Silver Birch Logs (Lenticel Bark)",
        55.0f,
        0.52f, 0.40f, 0.28f,
        RawMaterialForm::HALF_SPLIT_BIRCH_LOGS,
        Vec4(0.86f, 0.85f, 0.80f, 1.0f),
        Vec4(0.88f, 0.74f, 0.50f, 1.0f),
        0.78f,
        0.0f,
        0.55f
    },
    {
        CargoID::STACKED_CORDWOOD_RICK,
        "STACKED_CORDWOOD_RICK",
        "Cross-Stacked Cribbed Cordwood Timber Rick",
        380.0f,
        1.20f, 0.50f, 0.90f,
        RawMaterialForm::CROSS_STACKED_CORDWOOD_RICK,
        Vec4(0.46f, 0.33f, 0.22f, 1.0f),
        Vec4(0.82f, 0.66f, 0.43f, 1.0f),
        0.88f,
        0.0f,
        0.68f
    },
    {
        CargoID::WOODEN_RAILWAY_SLEEPER,
        "WOODEN_RAILWAY_SLEEPER",
        "Chamfered Hardwood Sleeper Beam (Creosote)",
        88.0f,
        2.60f, 0.26f, 0.16f,
        RawMaterialForm::CHAMFERED_RAILWAY_SLEEPER,
        Vec4(0.24f, 0.18f, 0.14f, 1.0f),
        Vec4(0.55f, 0.42f, 0.28f, 1.0f),
        0.80f,
        0.04f,
        0.48f
    },
    {
        CargoID::CHOPPED_KINDLING_BUNDLE,
        "CHOPPED_KINDLING_BUNDLE",
        "Twine-Bound Split Pine Kindling Bundle",
        14.5f,
        0.38f, 0.28f, 0.28f,
        RawMaterialForm::CONSTRICTED_KINDLING_BUNDLE,
        Vec4(0.85f, 0.71f, 0.46f, 1.0f),
        Vec4(0.55f, 0.38f, 0.20f, 1.0f),
        0.74f,
        0.0f,
        0.38f
    },
    {
        CargoID::TIMBER_STUMP_BLOCKS,
        "TIMBER_STUMP_BLOCKS",
        "Axe-Flared Butcher Hardwood Stump Block",
        96.0f,
        0.58f, 0.58f, 0.55f,
        RawMaterialForm::FLARED_BUTCHER_STUMP_BLOCK,
        Vec4(0.38f, 0.27f, 0.18f, 1.0f),
        Vec4(0.83f, 0.69f, 0.46f, 1.0f),
        0.90f,
        0.0f,
        0.82f
    }
};

const std::vector<RawVariantConfig>& RawMaterialsVariants::GetAllVariants() {
    return kRawVariants;
}

const RawVariantConfig* RawMaterialsVariants::FindVariant(CargoID id) {
    for (const auto& v : kRawVariants) {
        if (v.id == id) return &v;
    }
    return nullptr;
}

CargoDef RawMaterialsVariants::CreateCargoDef(const RawVariantConfig& cfg) {
    CargoDef def;
    def.id = cfg.id;
    def.name = cfg.displayName;
    def.category = "Raw Materials";
    def.packingType = CargoPackingType::RAW_STACK;
    def.weight_kg = cfg.nominalWeightKg;
    def.dimensions = Vec3(cfg.lengthMeters, cfg.widthMeters, cfg.heightMeters);
    def.volume_m3 = cfg.lengthMeters * cfg.widthMeters * cfg.heightMeters * 0.65f;
    def.density_kg_m3 = def.weight_kg / std::max(0.001f, def.volume_m3);
    def.max_stack_load_kg = 5000.0f;
    def.is_liquid_or_bulk = (cfg.form == RawMaterialForm::BULK_COAL_PILE || cfg.form == RawMaterialForm::AGGREGATE_GRAVEL_MOUND);
    def.is_perishable = false;

    def.albedo_format = VK_FORMAT_BC7_SRGB_BLOCK;
    def.normal_format = VK_FORMAT_BC5_UNORM_BLOCK;
    def.orm_format = VK_FORMAT_BC7_UNORM_BLOCK;
    def.texture_width = 1024;
    def.texture_height = 1024;

    def.base_color = cfg.primaryColor;
    def.roughness = cfg.roughness;
    def.metallic = cfg.metallic;
    def.opacity_bias = 1.0f;

    def.label_text = cfg.displayName;
    def.brand_id = cfg.variantCode;
    def.label_color = cfg.secondaryColor;

    def.wrinkle_or_corrugation = cfg.barkOrGritRoughness;
    return def;
}

} // namespace sphere::cargo
