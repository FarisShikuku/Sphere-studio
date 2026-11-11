#include "BaggedGoodsVariants.h"

namespace sphere::cargo {

static const std::vector<BagVariantConfig> kBagVariants = {
    {
        CargoID::SUGAR_BAG_50KG,
        "SUGAR_50KG",
        "Refined White Sugar 50kg (Woven PP)",
        50.0f,
        0.82f, 0.48f, 0.22f,
        BagMaterialType::WOVEN_POLYPROPYLENE,
        Vec4(0.96f, 0.96f, 0.97f, 1.0f),
        Vec4(0.08f, 0.28f, 0.82f, 1.0f),
        "PURE WHITE SUGAR 50KG",
        0.65f,
        0.32f
    },
    {
        CargoID::MAIZE_100KG,
        "MAIZE_100KG",
        "Yellow Maize Corn 100kg (Heavy Bulk PP)",
        100.0f,
        1.05f, 0.58f, 0.32f,
        BagMaterialType::WOVEN_POLYPROPYLENE,
        Vec4(0.92f, 0.86f, 0.68f, 1.0f),
        Vec4(0.12f, 0.45f, 0.18f, 1.0f),
        "GRADE A YELLOW MAIZE 100KG",
        0.72f,
        0.45f
    },
    {
        CargoID::CEMENT_50KG,
        "CEMENT_50KG",
        "Portland Construction Cement 50kg (Kraft)",
        50.0f,
        0.65f, 0.44f, 0.16f,
        BagMaterialType::MULTIWALL_KRAFT_PAPER,
        Vec4(0.78f, 0.72f, 0.62f, 1.0f),
        Vec4(0.75f, 0.15f, 0.12f, 1.0f),
        "PORTLAND CEMENT 42.5N",
        0.85f,
        0.18f
    },
    {
        CargoID::FERTILIZER_25KG,
        "FERTILIZER_25KG",
        "Agricultural NPK Fertilizer 25kg (Laminated)",
        25.0f,
        0.62f, 0.38f, 0.15f,
        BagMaterialType::LAMINATED_PLASTIC,
        Vec4(0.20f, 0.55f, 0.28f, 1.0f),
        Vec4(0.98f, 0.92f, 0.15f, 1.0f),
        "BIO-AGRI NPK 17-17-17",
        0.42f,
        0.24f
    },
    {
        CargoID::RICE_25KG,
        "RICE_25KG",
        "Long Grain White Rice 25kg (Fine Weave)",
        25.0f,
        0.58f, 0.36f, 0.18f,
        BagMaterialType::WOVEN_POLYPROPYLENE,
        Vec4(0.98f, 0.97f, 0.94f, 1.0f),
        Vec4(0.85f, 0.12f, 0.15f, 1.0f),
        "SUPER BASMATI RICE 25KG",
        0.60f,
        0.28f
    },
    {
        CargoID::COFFEE_BEAN_60KG,
        "COFFEE_BEAN_60KG",
        "Arabica Green Coffee Bean 60kg (Jute Sack)",
        60.0f,
        0.88f, 0.52f, 0.26f,
        BagMaterialType::ROUGH_JUTE_HESSIAN,
        Vec4(0.62f, 0.50f, 0.36f, 1.0f),
        Vec4(0.18f, 0.12f, 0.08f, 1.0f),
        "ORIGIN ARABICA GRADE 1 - 60KG",
        0.92f,
        0.52f
    },
    {
        CargoID::TEA_LEAF_50KG,
        "TEA_LEAF_50KG",
        "Black CTC Tea Leaf 50kg (Foil Kraft)",
        50.0f,
        0.78f, 0.46f, 0.24f,
        BagMaterialType::FOIL_LINED_KRAFT,
        Vec4(0.70f, 0.62f, 0.50f, 1.0f),
        Vec4(0.08f, 0.35f, 0.22f, 1.0f),
        "EXPORT BLACK TEA CTC 50KG",
        0.80f,
        0.22f
    }
};

const std::vector<BagVariantConfig>& BaggedGoodsVariants::GetAllVariants() {
    return kBagVariants;
}

const BagVariantConfig* BaggedGoodsVariants::FindVariant(CargoID id) {
    for (const auto& v : kBagVariants) {
        if (v.id == id) return &v;
    }
    return nullptr;
}

CargoDef BaggedGoodsVariants::CreateCargoDef(const BagVariantConfig& cfg) {
    CargoDef def;
    def.id = cfg.id;
    def.name = cfg.displayName;
    def.category = "Bagged Goods";
    def.packingType = CargoPackingType::BAG;
    def.weight_kg = cfg.nominalWeightKg;
    def.dimensions = Vec3(cfg.lengthMeters, cfg.widthMeters, cfg.heightMeters);
    def.volume_m3 = cfg.lengthMeters * cfg.widthMeters * cfg.heightMeters * 0.78f; // rounded sack packing factor
    def.density_kg_m3 = def.weight_kg / std::max(0.001f, def.volume_m3);
    def.max_stack_load_kg = cfg.nominalWeightKg * 16.0f;
    def.is_liquid_or_bulk = false;
    def.is_perishable = false;

    def.albedo_format = VK_FORMAT_BC7_SRGB_BLOCK;
    def.normal_format = VK_FORMAT_BC5_UNORM_BLOCK;
    def.orm_format = VK_FORMAT_BC7_UNORM_BLOCK;
    def.texture_width = 1024;
    def.texture_height = 1024;

    def.base_color = cfg.primaryColor;
    def.roughness = cfg.weaveRoughness;
    def.metallic = 0.0f;
    def.opacity_bias = 1.0f;

    def.label_text = cfg.brandText;
    def.brand_id = cfg.variantCode;
    def.label_color = cfg.labelColor;

    def.wrinkle_or_corrugation = cfg.wrinkleAmplitude;
    return def;
}

} // namespace sphere::cargo
