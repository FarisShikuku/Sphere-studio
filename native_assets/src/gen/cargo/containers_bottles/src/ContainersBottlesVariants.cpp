#include "ContainersBottlesVariants.h"

namespace sphere::cargo {

static const std::vector<BottleVariantConfig> kBottleVariants = {
    {
        CargoID::GLASS_BOTTLE_BEER,
        "GLASS_BOTTLE_BEER",
        "Amber Glass Beverage Bottle 330ml (Crown Cap)",
        0.58f,
        0.33f,
        0.062f,
        0.228f,
        BottleSubstrate::AMBER_BOROSILICATE_GLASS,
        Vec4(0.38f, 0.22f, 0.08f, 0.95f), // Rich amber glass
        Vec4(0.92f, 0.88f, 0.22f, 1.0f),
        "PREMIUM CRAFT PILSNER 330ML",
        0.35f, // Translucent glass
        0.08f, // Ultra smooth glass mirror
        0.0f,
        0.38f
    },
    {
        CargoID::PLASTIC_BOTTLE_SODA,
        "PLASTIC_BOTTLE_SODA",
        "PET Carbonated Soft Drink Bottle 2.0L",
        2.08f,
        2.00f,
        0.108f,
        0.325f,
        BottleSubstrate::CLEAR_POLYETHYLENE_TEREPHTHALATE,
        Vec4(0.25f, 0.08f, 0.05f, 0.98f), // Cola dark liquid
        Vec4(0.88f, 0.12f, 0.12f, 1.0f),
        "SPARKLING COLA 2L",
        0.45f,
        0.12f,
        0.0f,
        0.28f
    },
    {
        CargoID::PLASTIC_BOTTLE_WATER,
        "PLASTIC_BOTTLE_WATER",
        "Clear Spring Mineral Water PET Bottle 500ml",
        0.52f,
        0.50f,
        0.065f,
        0.210f,
        BottleSubstrate::CLEAR_POLYETHYLENE_TEREPHTHALATE,
        Vec4(0.92f, 0.96f, 1.0f, 0.95f), // Clear crystal water
        Vec4(0.12f, 0.45f, 0.88f, 1.0f),
        "NATURAL SPRING WATER 500ML",
        0.18f, // High transparency
        0.05f,
        0.0f,
        0.25f
    },
    {
        CargoID::JAR_GLASS,
        "JAR_GLASS",
        "Wide-Mouth Glass Food Preserve Jar 500ml",
        0.75f,
        0.50f,
        0.088f,
        0.135f,
        BottleSubstrate::CLEAR_SODA_LIME_GLASS_JAR,
        Vec4(0.95f, 0.98f, 0.98f, 1.0f),
        Vec4(0.18f, 0.65f, 0.32f, 1.0f),
        "ORGANIC HONEY PRESERVE 500G",
        0.25f,
        0.06f,
        0.0f,
        0.75f // wide mouth neck
    },
    {
        CargoID::TIN_CAN,
        "TIN_CAN",
        "Three-Piece Sanitary Tinplate Steel Can 400g",
        0.46f,
        0.40f,
        0.075f,
        0.112f,
        BottleSubstrate::ELECTROLYTIC_TINPLATE_STEEL_CAN,
        Vec4(0.90f, 0.90f, 0.92f, 1.0f), // Tinplate metallic body
        Vec4(0.92f, 0.15f, 0.15f, 1.0f),
        "WHOLE PEELED TOMATOES 400G",
        1.0f,  // Opaque
        0.30f,
        0.90f, // Metal
        1.0f
    }
};

const std::vector<BottleVariantConfig>& ContainersBottlesVariants::GetAllVariants() {
    return kBottleVariants;
}

const BottleVariantConfig* ContainersBottlesVariants::FindVariant(CargoID id) {
    for (const auto& v : kBottleVariants) {
        if (v.id == id) return &v;
    }
    return nullptr;
}

CargoDef ContainersBottlesVariants::CreateCargoDef(const BottleVariantConfig& cfg) {
    CargoDef def;
    def.id = cfg.id;
    def.name = cfg.displayName;
    def.category = "Containers & Bottles";
    def.packingType = CargoPackingType::BOTTLE;
    def.weight_kg = cfg.nominalWeightKg;
    def.dimensions = Vec3(cfg.baseDiameterMeters, cfg.baseDiameterMeters, cfg.totalHeightMeters);
    def.volume_m3 = cfg.volumeLiters * 0.001f;
    def.density_kg_m3 = def.weight_kg / std::max(0.001f, def.volume_m3);
    def.max_stack_load_kg = 150.0f;
    def.is_liquid_or_bulk = false;
    def.is_perishable = false;

    def.albedo_format = VK_FORMAT_BC7_SRGB_BLOCK;
    def.normal_format = VK_FORMAT_BC5_UNORM_BLOCK;
    def.orm_format = VK_FORMAT_BC7_UNORM_BLOCK;
    def.texture_width = 1024;
    def.texture_height = 1024;

    def.base_color = cfg.liquidColor;
    def.roughness = cfg.roughness;
    def.metallic = cfg.metallic;
    def.opacity_bias = cfg.opacityBias;

    def.label_text = cfg.brandText;
    def.brand_id = cfg.variantCode;
    def.label_color = cfg.labelColor;

    def.wrinkle_or_corrugation = 0.05f;
    return def;
}

} // namespace sphere::cargo
