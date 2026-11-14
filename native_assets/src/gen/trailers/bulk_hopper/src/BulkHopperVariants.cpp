#include "BulkHopperVariants.h"

namespace sphere::trailers::hopper {
const std::vector<BulkHopperVariantConfig>& BulkHopperVariants::GetAll() {
    static const std::vector<BulkHopperVariantConfig> kVariants = {
        {TrailerID::HOPPER_GRAIN, "Twin-Hopper Grain Bulker", 5600.f, 34000.f, Vec3(12.8f, 2.55f, 3.65f), false, Vec4(0.84f, 0.86f, 0.89f, 1.f), Vec4(0.14f, 0.48f, 0.24f, 1.f), "AGRI GRAIN 55M3"},
        {TrailerID::HOPPER_CEMENT_POWDER, "Pneumatic Cement Silo Hopper", 6100.f, 35000.f, Vec3(11.8f, 2.55f, 3.85f), true, Vec4(0.91f, 0.92f, 0.94f, 1.f), Vec4(0.85f, 0.28f, 0.10f, 1.f), "CEM-SILO 38M3"},
        {TrailerID::HOPPER_AGGREGATE_TIPPER, "End-Dump Aggregate Tipper", 6800.f, 36000.f, Vec3(10.6f, 2.55f, 3.35f), false, Vec4(0.86f, 0.54f, 0.08f, 1.f), Vec4(0.20f, 0.22f, 0.25f, 1.f), "HARDOX TIPPER"},
        {TrailerID::HOPPER_SIDE_TIPPER, "Quarry Side-Tipper Tub", 7200.f, 38000.f, Vec3(11.4f, 2.55f, 3.25f), false, Vec4(0.75f, 0.18f, 0.14f, 1.f), Vec4(0.18f, 0.20f, 0.22f, 1.f), "SIDE-TIPPER 42T"}
    };
    return kVariants;
}
const BulkHopperVariantConfig* BulkHopperVariants::FindVariant(TrailerID id) {
    for (const auto& v : GetAll()) if (v.id == id) return &v;
    return nullptr;
}
TrailerDef BulkHopperVariants::CreateTrailerDef(const BulkHopperVariantConfig& cfg) {
    TrailerDef d;
    d.id = cfg.id; d.name = cfg.name; d.category = "Bulk Hoppers";
    d.hitch_type = HitchType::FIFTH_WHEEL_KINGPIN;
    d.empty_weight_kg = cfg.emptyWeightKg; d.max_payload_kg = cfg.maxPayloadKg;
    d.dimensions = cfg.dims; d.volume_m3 = 48.0f;
    d.deck_height_m = 1.25f; d.axle_count = 3;
    d.base_color = cfg.baseColor; d.accent_color = cfg.accentColor;
    d.metallic = 0.58f; d.roughness = 0.36f; d.label_text = cfg.labelText;
    return d;
}
} // namespace sphere::trailers::hopper
