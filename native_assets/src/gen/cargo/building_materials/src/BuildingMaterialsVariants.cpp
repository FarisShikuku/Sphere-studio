#include "BuildingMaterialsVariants.h"
#include <algorithm>

namespace sphere::cargo {

static const std::vector<BuildingMaterialVariantConfig> kBuildingMaterialVariants = {
    {
        CargoID::STEEL_IBEAM_6M,
        "STEEL_IBEAM_6M",
        "Structural Wide-Flange Steel I-Beam 6m (HEA 240)",
        362.0f,
        6.00f, 0.24f, 0.23f,
        BuildingMaterialForm::STRUCTURAL_IBEAM,
        Vec4(0.32f, 0.36f, 0.41f, 1.0f),
        Vec4(0.92f, 0.35f, 0.05f, 1.0f),
        0.38f, 0.94f, 1.0f, false
    },
    {
        CargoID::TIMBER_GLULAM_BEAM,
        "TIMBER_GLULAM_BEAM",
        "Laminated Glulam Structural Roof Beam 4.8m (GL28h)",
        195.0f,
        4.80f, 0.16f, 0.32f,
        BuildingMaterialForm::GLULAM_TIMBER_BEAM,
        Vec4(0.84f, 0.68f, 0.45f, 1.0f),
        Vec4(0.42f, 0.27f, 0.15f, 1.0f),
        0.72f, 0.0f, 1.0f, false
    },
    {
        CargoID::SILICA_SAND_HEAP,
        "SILICA_SAND_HEAP",
        "Washed River & Masonry Silica Sand Heap (1.5m3)",
        2400.0f,
        1.85f, 1.85f, 0.68f,
        BuildingMaterialForm::WASHED_SAND_HEAP,
        Vec4(0.84f, 0.74f, 0.54f, 1.0f),
        Vec4(0.68f, 0.57f, 0.38f, 1.0f),
        0.93f, 0.04f, 1.0f, true
    },
    {
        CargoID::SILICA_SAND_FIBC_1T,
        "SILICA_SAND_FIBC_1T",
        "FIBC Builder's Sharp Sand Bulk Bag (1000kg)",
        1000.0f,
        0.90f, 0.90f, 1.10f,
        BuildingMaterialForm::SAND_FIBC_TOTE,
        Vec4(0.93f, 0.94f, 0.96f, 1.0f),
        Vec4(0.05f, 0.42f, 0.80f, 1.0f),
        0.68f, 0.0f, 1.0f, false
    },
    {
        CargoID::FLOAT_GLASS_STILLAGE,
        "FLOAT_GLASS_STILLAGE",
        "Tempered Architectural Float Glass A-Frame Pack",
        890.0f,
        2.40f, 0.95f, 1.45f,
        BuildingMaterialForm::FLOAT_GLASS_RACK,
        Vec4(0.56f, 0.84f, 0.80f, 1.0f),
        Vec4(0.18f, 0.25f, 0.32f, 1.0f),
        0.06f, 0.12f, 1.0f, false
    },
    {
        CargoID::HOLLOW_CONCRETE_CMU,
        "HOLLOW_CONCRETE_CMU",
        "Hollow Core Concrete Masonry Blocks (CMU Pallet)",
        920.0f,
        1.20f, 0.80f, 0.82f,
        BuildingMaterialForm::HOLLOW_CMU_PALLET,
        Vec4(0.56f, 0.58f, 0.60f, 1.0f),
        Vec4(0.36f, 0.38f, 0.40f, 1.0f),
        0.92f, 0.0f, 1.0f, false
    },
    {
        CargoID::PRECAST_CONCRETE_SLAB,
        "PRECAST_CONCRETE_SLAB",
        "Prestressed Hollow-Core Concrete Deck Slab 3.6m",
        1180.0f,
        3.60f, 1.20f, 0.22f,
        BuildingMaterialForm::PRECAST_HOLLOW_SLAB,
        Vec4(0.66f, 0.67f, 0.68f, 1.0f),
        Vec4(0.28f, 0.30f, 0.34f, 1.0f),
        0.86f, 0.05f, 1.0f, false
    },
    {
        CargoID::GYPSUM_DRYWALL_STACK,
        "GYPSUM_DRYWALL_STACK",
        "Moisture-Resistant Gypsum Drywall Board Stack",
        640.0f,
        2.40f, 1.20f, 0.32f,
        BuildingMaterialForm::GYPSUM_DRYWALL_STACK,
        Vec4(0.24f, 0.64f, 0.58f, 1.0f),
        Vec4(0.93f, 0.94f, 0.95f, 1.0f),
        0.80f, 0.0f, 1.0f, false
    },
    {
        CargoID::CORRUGATED_ROOF_SHEETS,
        "CORRUGATED_ROOF_SHEETS",
        "Galvanized Corrugated Steel Roofing Sheet Pack",
        310.0f,
        2.50f, 0.92f, 0.16f,
        BuildingMaterialForm::CORRUGATED_STEEL_ROOF,
        Vec4(0.68f, 0.72f, 0.76f, 1.0f),
        Vec4(0.18f, 0.24f, 0.30f, 1.0f),
        0.28f, 0.90f, 1.0f, false
    },
    {
        CargoID::TERRA_ROOF_TILES_PALLET,
        "TERRA_ROOF_TILES_PALLET",
        "Interlocking Barrel Terracotta Clay Roof Tiles",
        780.0f,
        1.10f, 0.85f, 0.72f,
        BuildingMaterialForm::TERRACOTTA_TILE_BUNDLE,
        Vec4(0.75f, 0.34f, 0.20f, 1.0f),
        Vec4(0.46f, 0.22f, 0.14f, 1.0f),
        0.78f, 0.0f, 1.0f, false
    }
};

const std::vector<BuildingMaterialVariantConfig>& BuildingMaterialsVariants::GetAllVariants() {
    return kBuildingMaterialVariants;
}

const BuildingMaterialVariantConfig* BuildingMaterialsVariants::FindVariant(CargoID id) {
    for (const auto& v : kBuildingMaterialVariants) {
        if (v.id == id) return &v;
    }
    return nullptr;
}

CargoDef BuildingMaterialsVariants::CreateCargoDef(const BuildingMaterialVariantConfig& cfg) {
    CargoDef def;
    def.id = cfg.id;
    def.name = cfg.displayName;
    def.category = "Building Materials";
    def.packingType = cfg.isBulkGranular ? CargoPackingType::RAW_STACK : CargoPackingType::PALLETIZED;
    def.weight_kg = cfg.nominalWeightKg;
    def.dimensions = Vec3(cfg.lengthMeters, cfg.widthMeters, cfg.heightMeters);
    def.volume_m3 = cfg.lengthMeters * cfg.widthMeters * cfg.heightMeters;
    def.density_kg_m3 = def.weight_kg / std::max(0.001f, def.volume_m3);
    def.max_stack_load_kg = 5000.0f;
    def.is_liquid_or_bulk = cfg.isBulkGranular;
    def.is_perishable = false;

    def.albedo_format = VK_FORMAT_BC7_SRGB_BLOCK;
    def.normal_format = VK_FORMAT_BC5_UNORM_BLOCK;
    def.orm_format = VK_FORMAT_BC7_UNORM_BLOCK;
    def.texture_width = 1024;
    def.texture_height = 1024;

    def.base_color = cfg.primaryColor;
    def.roughness = cfg.roughness;
    def.metallic = cfg.metallic;
    def.opacity_bias = cfg.opacityBias;

    def.label_text = cfg.displayName;
    def.brand_id = cfg.variantCode;
    def.label_color = cfg.secondaryColor;
    def.wrinkle_or_corrugation = 0.25f;
    return def;
}

} // namespace sphere::cargo
