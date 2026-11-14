#include "CarCarrierVariants.h"

namespace sphere::trailers::carcarrier {
const std::vector<CarCarrierVariantConfig>& CarCarrierVariants::GetAll() {
    static const std::vector<CarCarrierVariantConfig> kVariants = {
        {TrailerID::CARCARRIER_SINGLE_DECK, "Single-Deck 3-Car Transporter", 4900.f, 12000.f, Vec3(12.2f, 2.52f, 2.25f), false, false, Vec4(0.14f, 0.36f, 0.65f, 1.f), "3-CAR OPEN"},
        {TrailerID::CARCARRIER_DOUBLE_DECK, "Hydraulic 8-Car Double-Deck Stinger", 9200.f, 22000.f, Vec3(16.4f, 2.55f, 4.10f), true, false, Vec4(0.82f, 0.20f, 0.14f, 1.f), "8-CAR STINGER"},
        {TrailerID::CARCARRIER_OPEN_TRANSPORTER, "Open Steel Track Car Hauler", 6800.f, 19000.f, Vec3(14.5f, 2.55f, 3.85f), true, false, Vec4(0.18f, 0.52f, 0.34f, 1.f), "6-CAR TRACK"},
        {TrailerID::CARCARRIER_ENCLOSED, "Enclosed Exotic Motorsport Carrier", 8800.f, 20000.f, Vec3(16.15f, 2.59f, 4.15f), true, true, Vec4(0.11f, 0.13f, 0.17f, 1.f), "MOTORSPORT PRO"}
    };
    return kVariants;
}
const CarCarrierVariantConfig* CarCarrierVariants::FindVariant(TrailerID id) {
    for (const auto& v : GetAll()) if (v.id == id) return &v;
    return nullptr;
}
TrailerDef CarCarrierVariants::CreateTrailerDef(const CarCarrierVariantConfig& cfg) {
    TrailerDef d;
    d.id = cfg.id; d.name = cfg.name; d.category = "Car Carriers";
    d.hitch_type = HitchType::FIFTH_WHEEL_KINGPIN;
    d.empty_weight_kg = cfg.emptyWeightKg; d.max_payload_kg = cfg.maxPayloadKg;
    d.dimensions = cfg.dims; d.volume_m3 = 88.0f;
    d.deck_height_m = 0.92f; d.axle_count = 2;
    d.wheel_radius_m = 0.42f;
    d.base_color = cfg.baseColor; d.accent_color = Vec4(0.88f, 0.89f, 0.91f, 1.f);
    d.metallic = 0.72f; d.roughness = 0.38f; d.label_text = cfg.labelText;
    return d;
}
} // namespace sphere::trailers::carcarrier
