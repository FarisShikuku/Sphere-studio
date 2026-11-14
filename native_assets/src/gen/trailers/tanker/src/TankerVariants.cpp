#include "TankerVariants.h"

namespace sphere::trailers::tanker {

const std::vector<TankerVariantConfig>& TankerVariants::GetAll() {
    static const std::vector<TankerVariantConfig> kVariants = {
        {TrailerID::TANKER_WATER, "Potable Water Tanker 30kL", 7100.f, 30000.f, Vec3(12.2f, 2.50f, 3.65f), 2, false, Vec4(0.18f, 0.45f, 0.72f, 1.f), Vec4(0.86f, 0.89f, 0.92f, 1.f), 0.24f, 0.65f, "POTABLE WATER"},
        {TrailerID::TANKER_FUEL_MULTI_COMPARTMENT, "Multi-Compartment Fuel Tanker", 7600.f, 34000.f, Vec3(13.2f, 2.55f, 3.72f), 5, false, Vec4(0.86f, 0.88f, 0.90f, 1.f), Vec4(0.85f, 0.16f, 0.14f, 1.f), 0.18f, 0.92f, "FLAMMABLE UN1203"},
        {TrailerID::TANKER_MILK, "Insulated Stainless Milk Tanker", 7400.f, 29000.f, Vec3(12.4f, 2.50f, 3.65f), 3, false, Vec4(0.92f, 0.94f, 0.96f, 1.f), Vec4(0.12f, 0.42f, 0.75f, 1.f), 0.14f, 0.95f, "GRADE A DAIRY"},
        {TrailerID::TANKER_CHEMICAL, "Corrosive Acid Chemical Tanker", 8200.f, 31000.f, Vec3(12.6f, 2.50f, 3.62f), 1, true, Vec4(0.76f, 0.79f, 0.82f, 1.f), Vec4(0.92f, 0.65f, 0.06f, 1.f), 0.22f, 0.88f, "CORROSIVE UN1789"},
        {TrailerID::TANKER_LPG_GAS, "Pressurized LPG Gas Tanker", 10400.f, 24000.f, Vec3(12.8f, 2.50f, 3.80f), 1, true, Vec4(0.94f, 0.94f, 0.92f, 1.f), Vec4(0.86f, 0.20f, 0.12f, 1.f), 0.28f, 0.35f, "LPG UN1075"},
        {TrailerID::TANKER_POWDER, "Pressurized Dry Powder Tanker", 7900.f, 32000.f, Vec3(12.9f, 2.55f, 3.85f), 3, true, Vec4(0.84f, 0.85f, 0.87f, 1.f), Vec4(0.18f, 0.50f, 0.35f, 1.f), 0.26f, 0.78f, "SILO BULK 2.0 BAR"}
    };
    return kVariants;
}

const TankerVariantConfig* TankerVariants::FindVariant(TrailerID id) {
    for (const auto& v : GetAll()) if (v.id == id) return &v;
    return nullptr;
}

TrailerDef TankerVariants::CreateTrailerDef(const TankerVariantConfig& cfg) {
    TrailerDef d;
    d.id = cfg.id;
    d.name = cfg.name;
    d.category = "Tankers";
    d.hitch_type = HitchType::FIFTH_WHEEL_KINGPIN;
    d.empty_weight_kg = cfg.emptyWeightKg;
    d.max_payload_kg = cfg.maxPayloadKg;
    d.dimensions = cfg.dims;
    d.volume_m3 = 36.0f;
    d.deck_height_m = 1.25f;
    d.axle_count = 3;
    d.has_lift_axle = true;
    d.base_color = cfg.baseColor;
    d.accent_color = cfg.accentColor;
    d.roughness = cfg.roughness;
    d.metallic = cfg.metallic;
    d.label_text = cfg.labelText;
    return d;
}

} // namespace sphere::trailers::tanker
