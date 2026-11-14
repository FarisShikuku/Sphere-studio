#include "DollyVariants.h"

namespace sphere::trailers::dolly {
const std::vector<DollyVariantConfig>& DollyVariants::GetAll() {
    static const std::vector<DollyVariantConfig> kVariants = {
        {TrailerID::DOLLY_CONVERTER, "Twin-Axle Road-Train Converter Dolly", 2650.f, 22000.f, Vec3(4.30f, 2.48f, 1.35f), 2, Vec4(0.84f, 0.22f, 0.14f, 1.f), "ROAD TRAIN DOLLY"},
        {TrailerID::DOLLY_A_DOLLY, "Single-Axle A-Frame Converter Dolly", 1550.f, 11000.f, Vec3(3.60f, 2.45f, 1.30f), 1, Vec4(0.16f, 0.36f, 0.62f, 1.f), "A-DOLLY 11T"},
        {TrailerID::DOLLY_JEEP, "Heavy Haulage Tandem Jeep Dolly", 4200.f, 36000.f, Vec3(5.20f, 2.75f, 1.38f), 2, Vec4(0.88f, 0.58f, 0.08f, 1.f), "HEAVY JEEP 36T"}
    };
    return kVariants;
}
const DollyVariantConfig* DollyVariants::FindVariant(TrailerID id) {
    for (const auto& v : GetAll()) if (v.id == id) return &v;
    return nullptr;
}
TrailerDef DollyVariants::CreateTrailerDef(const DollyVariantConfig& cfg) {
    TrailerDef d;
    d.id = cfg.id; d.name = cfg.name; d.category = "Dollies";
    d.hitch_type = HitchType::PINTLE_RING_DRAWBAR;
    d.empty_weight_kg = cfg.emptyWeightKg; d.max_payload_kg = cfg.maxPayloadKg;
    d.dimensions = cfg.dims; d.volume_m3 = 2.0f;
    d.deck_height_m = 1.18f; d.axle_count = cfg.axleCount;
    d.base_color = cfg.baseColor; d.accent_color = Vec4(0.18f, 0.20f, 0.23f, 1.f);
    d.metallic = 0.75f; d.roughness = 0.44f; d.label_text = cfg.labelText;
    return d;
}
} // namespace sphere::trailers::dolly
