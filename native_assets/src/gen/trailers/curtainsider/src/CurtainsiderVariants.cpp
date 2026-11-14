#include "CurtainsiderVariants.h"

namespace sphere::trailers::curtainsider {
const std::vector<CurtainsiderVariantConfig>& CurtainsiderVariants::GetAll() {
    static const std::vector<CurtainsiderVariantConfig> kVariants = {
        {TrailerID::CURTAINSIDER_STANDARD, "Standard EU Tautliner Curtainsider", 6400.f, 32000.f, Vec3(13.6f, 2.55f, 4.00f), 1.0f, Vec4(0.14f, 0.34f, 0.68f, 1.0f), Vec4(0.95f, 0.96f, 0.98f, 1.f), "EURO FREIGHT LINE"},
        {TrailerID::CURTAINSIDER_XL, "EN 12642-XL Certified Curtainsider", 6650.f, 32500.f, Vec3(13.6f, 2.55f, 4.05f), 1.0f, Vec4(0.76f, 0.16f, 0.14f, 1.0f), Vec4(0.96f, 0.78f, 0.12f, 1.f), "CODE XL CARGO"},
        {TrailerID::CURTAINSIDER_DOUBLE_DECK, "Double-Deck Lifting Curtainsider", 8400.f, 30000.f, Vec3(13.6f, 2.55f, 4.38f), 1.0f, Vec4(0.12f, 0.45f, 0.32f, 1.0f), Vec4(0.90f, 0.94f, 0.92f, 1.f), "4.38M MEGA DECK"},
        {TrailerID::CURTAINSIDER_SIDE_LOADING, "Mega-Cube Side-Loading Curtainsider", 6900.f, 31500.f, Vec3(13.6f, 2.55f, 4.15f), 1.0f, Vec4(0.24f, 0.28f, 0.36f, 1.0f), Vec4(0.22f, 0.74f, 0.92f, 1.f), "MEGA CUBE 100M3"}
    };
    return kVariants;
}
const CurtainsiderVariantConfig* CurtainsiderVariants::FindVariant(TrailerID id) {
    for (const auto& v : GetAll()) if (v.id == id) return &v;
    return nullptr;
}
TrailerDef CurtainsiderVariants::CreateTrailerDef(const CurtainsiderVariantConfig& cfg) {
    TrailerDef d;
    d.id = cfg.id; d.name = cfg.name; d.category = "Curtainsiders";
    d.hitch_type = HitchType::FIFTH_WHEEL_KINGPIN;
    d.empty_weight_kg = cfg.emptyWeightKg; d.max_payload_kg = cfg.maxPayloadKg;
    d.dimensions = cfg.dims; d.volume_m3 = 96.0f;
    d.deck_height_m = 1.25f; d.axle_count = 3;
    d.has_lift_axle = true;
    d.opacity_bias = cfg.curtainOpacity;
    d.base_color = cfg.curtainColor; d.accent_color = cfg.stripeColor;
    d.roughness = 0.62f; d.metallic = 0.12f; d.label_text = cfg.labelText;
    return d;
}
} // namespace sphere::trailers::curtainsider
