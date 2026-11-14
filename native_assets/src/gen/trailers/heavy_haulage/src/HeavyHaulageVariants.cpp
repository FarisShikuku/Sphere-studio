#include "HeavyHaulageVariants.h"

namespace sphere::trailers::heavy {
const std::vector<HeavyVariantConfig>& HeavyHaulageVariants::GetAll() {
    static const std::vector<HeavyVariantConfig> kVariants = {
        {TrailerID::HEAVY_LOWBOY, "Detachable Gooseneck 60T Lowboy", 13500.f, 65000.f, Vec3(16.2f, 3.00f, 1.55f), 0.58f, 4, Vec4(0.88f, 0.58f, 0.08f, 1.f), "60T DETACHABLE LOWBOY"},
        {TrailerID::HEAVY_MULTI_AXLE_MODULAR, "12-Axle Modular Heavy Platform", 26000.f, 180000.f, Vec3(18.5f, 3.20f, 1.45f), 1.05f, 6, Vec4(0.75f, 0.16f, 0.14f, 1.f), "SPMT MODULAR 180T"},
        {TrailerID::HEAVY_BLADE_TRANSPORTER, "Wind Turbine Blade Lifter Trailer", 15800.f, 45000.f, Vec3(22.0f, 2.85f, 2.40f), 1.15f, 4, Vec4(0.14f, 0.42f, 0.68f, 1.f), "WIND BLADE ADAPTER"},
        {TrailerID::HEAVY_TUNNEL_TRANSPORTER, "TBM Shield Tunnel Segment Hauler", 19200.f, 120000.f, Vec3(17.2f, 3.10f, 1.75f), 0.72f, 5, Vec4(0.24f, 0.27f, 0.31f, 1.f), "TBM HEAVY 120T"}
    };
    return kVariants;
}
const HeavyVariantConfig* HeavyHaulageVariants::FindVariant(TrailerID id) {
    for (const auto& v : GetAll()) if (v.id == id) return &v;
    return nullptr;
}
TrailerDef HeavyHaulageVariants::CreateTrailerDef(const HeavyVariantConfig& cfg) {
    TrailerDef d;
    d.id = cfg.id; d.name = cfg.name; d.category = "Heavy Haulage";
    d.hitch_type = HitchType::DETACHABLE_GOOSENECK;
    d.empty_weight_kg = cfg.emptyWeightKg; d.max_payload_kg = cfg.maxPayloadKg;
    d.dimensions = cfg.dims; d.volume_m3 = 90.0f;
    d.deck_height_m = cfg.deckHeightM; d.axle_count = cfg.axleCount;
    d.has_steerable_axle = true;
    d.suspension_type = SuspensionType::HYDRAULIC_MODULAR;
    d.wheel_radius_m = 0.40f;
    d.base_color = cfg.baseColor; d.accent_color = Vec4(0.15f, 0.17f, 0.20f, 1.f);
    d.metallic = 0.72f; d.roughness = 0.46f; d.label_text = cfg.labelText;
    return d;
}
} // namespace sphere::trailers::heavy
