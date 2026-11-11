#include "LiquidBulkVariants.h"

namespace sphere::cargo {

static const std::vector<LiquidVariantConfig> kLiquidVariants = {
    {
        CargoID::WATER_TANKER,
        "WATER_TANKER",
        "Potable Water Tanker Module (8,000L)",
        8600.0f,
        8000.0f,
        1.80f,
        4.50f, // length along X
        LiquidVesselType::BULK_ROAD_TANKER,
        Vec4(0.85f, 0.88f, 0.92f, 1.0f),
        Vec4(0.10f, 0.45f, 0.85f, 1.0f),
        "DRINKING WATER ONLY",
        0.75f,
        0.28f,
        4
    },
    {
        CargoID::OIL_DRUM_200L,
        "OIL_DRUM_200L",
        "Closed-Head Steel Chemical Drum (200L / 55 Gal)",
        215.0f,
        200.0f,
        0.58f,
        0.88f,
        LiquidVesselType::STEEL_DRUM_200L,
        Vec4(0.10f, 0.28f, 0.65f, 1.0f), // Classic industrial blue
        Vec4(0.95f, 0.65f, 0.10f, 1.0f),
        "UN 1A1/Y1.4/250",
        0.85f,
        0.35f,
        2
    },
    {
        CargoID::JERRICAN_20L,
        "JERRICAN_20L",
        "High-Density Polyethylene Jerrican 20L",
        22.5f,
        20.0f,
        0.28f,
        0.47f,
        LiquidVesselType::PLASTIC_JERRICAN,
        Vec4(0.95f, 0.82f, 0.10f, 1.0f), // High-vis fuel yellow
        Vec4(0.10f, 0.10f, 0.10f, 1.0f),
        "DIESEL FUEL 20L",
        0.0f,
        0.38f,
        0
    },
    {
        CargoID::MILK_CHURN,
        "MILK_CHURN",
        "Seamless Stainless Steel Milk Can 40L",
        45.0f,
        40.0f,
        0.36f,
        0.62f,
        LiquidVesselType::DAIRY_MILK_CHURN,
        Vec4(0.90f, 0.92f, 0.94f, 1.0f),
        Vec4(0.20f, 0.20f, 0.25f, 1.0f),
        "GRADE A RAW DAIRY",
        0.95f,
        0.20f,
        1
    },
    {
        CargoID::GAS_CYLINDER,
        "GAS_CYLINDER",
        "Liquid Petroleum Gas (LPG) Steel Cylinder 15kg",
        28.5f,
        35.0f,
        0.32f,
        0.68f,
        LiquidVesselType::PRESSURIZED_GAS_CYLINDER,
        Vec4(0.85f, 0.25f, 0.18f, 1.0f), // LPG Red
        Vec4(0.98f, 0.98f, 0.98f, 1.0f),
        "FLAMMABLE GAS 2.1",
        0.70f,
        0.42f,
        1
    }
};

const std::vector<LiquidVariantConfig>& LiquidBulkVariants::GetAllVariants() {
    return kLiquidVariants;
}

const LiquidVariantConfig* LiquidBulkVariants::FindVariant(CargoID id) {
    for (const auto& v : kLiquidVariants) {
        if (v.id == id) return &v;
    }
    return nullptr;
}

CargoDef LiquidBulkVariants::CreateCargoDef(const LiquidVariantConfig& cfg) {
    CargoDef def;
    def.id = cfg.id;
    def.name = cfg.displayName;
    def.category = "Liquid Bulk";
    def.packingType = CargoPackingType::DRUM;
    def.weight_kg = cfg.nominalWeightKg;
    def.dimensions = Vec3(cfg.diameterMeters, cfg.diameterMeters, cfg.heightMeters);
    def.volume_m3 = cfg.capacityLiters * 0.001f;
    def.density_kg_m3 = def.weight_kg / std::max(0.001f, def.volume_m3);
    def.max_stack_load_kg = (cfg.id == CargoID::OIL_DRUM_200L) ? 800.0f : 200.0f;
    def.is_liquid_or_bulk = true;
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

    def.label_text = cfg.hazchemCode;
    def.brand_id = cfg.variantCode;
    def.label_color = cfg.hazardDecalColor;

    def.wrinkle_or_corrugation = static_cast<float>(cfg.ribCount);
    return def;
}

} // namespace sphere::cargo
