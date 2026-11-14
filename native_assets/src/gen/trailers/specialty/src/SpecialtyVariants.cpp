#include "SpecialtyVariants.h"

namespace sphere::trailers::specialty {
const std::vector<SpecialtyVariantConfig>& SpecialtyVariants::GetAll() {
    static const std::vector<SpecialtyVariantConfig> kVariants = {
        {TrailerID::SPECIALTY_LIVESTOCK, "Triple-Deck Ventilated Livestock Hauler", 8800.f, 26000.f, Vec3(14.8f, 2.55f, 4.15f), 3, Vec4(0.84f, 0.86f, 0.89f, 1.f), Vec4(0.18f, 0.21f, 0.24f, 1.f), "LIVESTOCK POT 3-DECK"},
        {TrailerID::SPECIALTY_WASTE_TANKER, "Municipal Vacuum Waste Tanker", 8400.f, 28000.f, Vec3(11.8f, 2.52f, 3.65f), 3, Vec4(0.16f, 0.44f, 0.26f, 1.f), Vec4(0.92f, 0.65f, 0.08f, 1.f), "VACUUM TANKER"},
        {TrailerID::SPECIALTY_MILITARY_TRANSPORT, "Heavy Tactical Tank Transporter", 16400.f, 70000.f, Vec3(16.8f, 3.30f, 1.75f), 5, Vec4(0.32f, 0.36f, 0.24f, 1.f), Vec4(0.18f, 0.20f, 0.16f, 1.f), "HET M1000 70T"},
        {TrailerID::SPECIALTY_ARMORED_CAR, "Armored Escort Security Trailer", 9600.f, 18000.f, Vec3(12.2f, 2.55f, 3.65f), 2, Vec4(0.20f, 0.23f, 0.28f, 1.f), Vec4(0.75f, 0.14f, 0.14f, 1.f), "ARMORED CLASS B6"}
    };
    return kVariants;
}
const SpecialtyVariantConfig* SpecialtyVariants::FindVariant(TrailerID id) {
    for (const auto& v : GetAll()) if (v.id == id) return &v;
    return nullptr;
}
TrailerDef SpecialtyVariants::CreateTrailerDef(const SpecialtyVariantConfig& cfg) {
    TrailerDef d;
    d.id = cfg.id; d.name = cfg.name; d.category = "Specialty";
    d.hitch_type = HitchType::FIFTH_WHEEL_KINGPIN;
    d.empty_weight_kg = cfg.emptyWeightKg; d.max_payload_kg = cfg.maxPayloadKg;
    d.dimensions = cfg.dims; d.volume_m3 = 72.0f;
    d.deck_height_m = (cfg.id == TrailerID::SPECIALTY_MILITARY_TRANSPORT) ? 0.98f : 1.25f;
    d.axle_count = cfg.axleCount;
    d.base_color = cfg.baseColor; d.accent_color = cfg.accentColor;
    d.metallic = 0.68f; d.roughness = 0.48f; d.label_text = cfg.labelText;
    return d;
}
} // namespace sphere::trailers::specialty
