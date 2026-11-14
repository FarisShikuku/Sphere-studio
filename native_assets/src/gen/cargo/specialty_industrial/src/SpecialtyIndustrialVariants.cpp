#include "SpecialtyIndustrialVariants.h"

namespace sphere::cargo {

static const std::vector<SpecialtyVariantConfig> kSpecialtyVariants = {
    {
        CargoID::CEMENT_BAG_50KG,
        "CEMENT_BAG_50KG",
        "Industrial Rapid-Hardening Portland Cement 50kg",
        50.0f,
        0.65f, 0.44f, 0.16f,
        SpecialtyCategory::HEAVY_CEMENT_BAG,
        Vec4(0.75f, 0.72f, 0.65f, 1.0f),
        Vec4(0.12f, 0.12f, 0.14f, 1.0f),
        0.85f,
        0.0f,
        1
    },
    {
        CargoID::STEEL_PIPE_6M,
        "STEEL_PIPE_6M",
        "Seamless Schedule 40 Carbon Steel Pipe 6m",
        185.0f,
        6.00f, 0.22f, 0.22f,
        SpecialtyCategory::LONG_STEEL_PIPE,
        Vec4(0.28f, 0.30f, 0.35f, 1.0f),
        Vec4(0.95f, 0.85f, 0.15f, 1.0f), // Yellow protective end cap
        0.42f,
        0.95f,
        1
    },
    {
        CargoID::BRICK_PALLET,
        "BRICK_PALLET",
        "Shrink-Wrapped Pallet of Fired Clay Facing Bricks (400 Units)",
        1150.0f,
        1.20f, 0.80f, 0.95f,
        SpecialtyCategory::MASONRY_BRICK_PALLET,
        Vec4(0.72f, 0.32f, 0.22f, 1.0f), // Terracotta brick red
        Vec4(0.95f, 0.95f, 0.95f, 0.65f), // Translucent shrink wrap
        0.88f,
        0.0f,
        400
    },
    {
        CargoID::TYRE_STACK,
        "TYRE_STACK",
        "Heavy Commercial Truck All-Position Radial Tyre Stack (4 Units)",
        240.0f,
        1.08f, 1.08f, 1.20f,
        SpecialtyCategory::COMMERCIAL_TYRE_STACK,
        Vec4(0.12f, 0.12f, 0.13f, 1.0f), // Vulcanized carbon rubber
        Vec4(0.92f, 0.85f, 0.20f, 1.0f), // Quality inspection paint stripe
        0.75f,
        0.02f,
        4
    }
};

const std::vector<SpecialtyVariantConfig>& SpecialtyIndustrialVariants::GetAllVariants() {
    return kSpecialtyVariants;
}

const SpecialtyVariantConfig* SpecialtyIndustrialVariants::FindVariant(CargoID id) {
    for (const auto& v : kSpecialtyVariants) {
        if (v.id == id) return &v;
    }
    return nullptr;
}

CargoDef SpecialtyIndustrialVariants::CreateCargoDef(const SpecialtyVariantConfig& cfg) {
    CargoDef def;
    def.id = cfg.id;
    def.name = cfg.displayName;
    def.category = "Specialty Industrial";
    def.packingType = (cfg.id == CargoID::BRICK_PALLET) ? CargoPackingType::PALLETIZED : CargoPackingType::CRATE;
    def.weight_kg = cfg.nominalWeightKg;
    def.dimensions = Vec3(cfg.lengthMeters, cfg.widthMeters, cfg.heightMeters);
    def.volume_m3 = cfg.lengthMeters * cfg.widthMeters * cfg.heightMeters;
    def.density_kg_m3 = def.weight_kg / std::max(0.001f, def.volume_m3);
    def.max_stack_load_kg = 3500.0f;
    def.is_liquid_or_bulk = false;
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

    def.wrinkle_or_corrugation = 0.2f;
    return def;
}

} // namespace sphere::cargo
