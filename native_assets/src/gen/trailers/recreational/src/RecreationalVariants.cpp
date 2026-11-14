#include "RecreationalVariants.h"

namespace sphere::trailers::recreational {
const std::vector<RecreationalVariantConfig>& RecreationalVariants::GetAll() {
    static const std::vector<RecreationalVariantConfig> kVariants = {
        {TrailerID::RECREATION_BOAT_TRAILER, "Galvanized Keel-Roller Boat Trailer", 680.f, 2800.f, Vec3(7.40f, 2.25f, 1.35f), 2, Vec4(0.76f, 0.79f, 0.84f, 1.f), "MARINE ROLLER"},
        {TrailerID::RECREATION_ATV_TRAILER, "Dual-Quad Utility ATV Mesh Trailer", 420.f, 1800.f, Vec3(4.60f, 2.10f, 1.15f), 1, Vec4(0.18f, 0.21f, 0.25f, 1.f), "ATV UTILITY"},
        {TrailerID::RECREATION_CAMPER_TRAILER, "Expedition Off-Road Camper Trailer", 1650.f, 950.f, Vec3(6.20f, 2.20f, 2.55f), 1, Vec4(0.84f, 0.78f, 0.64f, 1.f), "OVERLAND CAMPER"},
        {TrailerID::RECREATION_UTILITY_TRAILER, "Tandem Box General Utility Trailer", 580.f, 2500.f, Vec3(5.20f, 2.15f, 1.25f), 2, Vec4(0.16f, 0.36f, 0.56f, 1.f), "UTILITY 2500KG"}
    };
    return kVariants;
}
const RecreationalVariantConfig* RecreationalVariants::FindVariant(TrailerID id) {
    for (const auto& v : GetAll()) if (v.id == id) return &v;
    return nullptr;
}
TrailerDef RecreationalVariants::CreateTrailerDef(const RecreationalVariantConfig& cfg) {
    TrailerDef d;
    d.id = cfg.id; d.name = cfg.name; d.category = "Recreational";
    d.hitch_type = HitchType::BALL_HITCH_50MM;
    d.empty_weight_kg = cfg.emptyWeightKg; d.max_payload_kg = cfg.maxPayloadKg;
    d.dimensions = cfg.dims; d.volume_m3 = 12.0f;
    d.deck_height_m = 0.65f; d.axle_count = cfg.axleCount;
    d.wheel_radius_m = 0.36f; d.tire_width_m = 0.24f;
    d.suspension_type = SuspensionType::LEAF_SPRING;
    d.base_color = cfg.baseColor; d.accent_color = Vec4(0.95f, 0.68f, 0.10f, 1.f);
    d.metallic = 0.62f; d.roughness = 0.35f; d.label_text = cfg.labelText;
    return d;
}
} // namespace sphere::trailers::recreational
