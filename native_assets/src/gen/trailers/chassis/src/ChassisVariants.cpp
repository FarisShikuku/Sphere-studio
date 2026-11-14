#include "ChassisVariants.h"

namespace sphere::trailers::chassis {
const std::vector<ChassisVariantConfig>& ChassisVariants::GetAll() {
    static const std::vector<ChassisVariantConfig> kVariants = {
        {TrailerID::CHASSIS_20FT, "20ft ISO Container Chassis", 3400.f, 30480.f, Vec3(7.20f, 2.48f, 1.32f), 2, false, false, Vec4(0.18f, 0.24f, 0.32f, 1.f), "20FT ISO SKELETAL"},
        {TrailerID::CHASSIS_40FT, "40ft Tri-Axle Container Chassis", 4800.f, 34000.f, Vec3(12.40f, 2.48f, 1.32f), 3, false, false, Vec4(0.16f, 0.20f, 0.25f, 1.f), "40FT INTERMODAL"},
        {TrailerID::CHASSIS_GOOSENECK, "Gooseneck High-Cube Chassis", 4600.f, 33000.f, Vec3(12.60f, 2.48f, 1.25f), 3, true, false, Vec4(0.14f, 0.34f, 0.54f, 1.f), "GOOSENECK 9'6\""},
        {TrailerID::CHASSIS_SLIDER, "Extendable Slider Container Chassis", 5200.f, 34000.f, Vec3(13.60f, 2.48f, 1.32f), 3, true, false, Vec4(0.68f, 0.18f, 0.15f, 1.f), "SLIDER 20-45FT"},
        {TrailerID::CHASSIS_TIPPING, "Hydraulic Tipping Container Chassis", 5900.f, 32000.f, Vec3(8.40f, 2.50f, 1.45f), 3, false, true, Vec4(0.82f, 0.48f, 0.08f, 1.f), "TIPPING BULK 20FT"}
    };
    return kVariants;
}
const ChassisVariantConfig* ChassisVariants::FindVariant(TrailerID id) {
    for (const auto& v : GetAll()) if (v.id == id) return &v;
    return nullptr;
}
TrailerDef ChassisVariants::CreateTrailerDef(const ChassisVariantConfig& cfg) {
    TrailerDef d;
    d.id = cfg.id; d.name = cfg.name; d.category = "Container Chassis";
    d.hitch_type = HitchType::FIFTH_WHEEL_KINGPIN;
    d.empty_weight_kg = cfg.emptyWeightKg; d.max_payload_kg = cfg.maxPayloadKg;
    d.dimensions = cfg.dims; d.volume_m3 = 67.7f;
    d.deck_height_m = 1.20f; d.axle_count = cfg.axleCount;
    d.base_color = cfg.baseColor; d.accent_color = Vec4(0.95f, 0.72f, 0.10f, 1.f);
    d.metallic = 0.72f; d.roughness = 0.44f; d.label_text = cfg.labelText;
    return d;
}
} // namespace sphere::trailers::chassis
