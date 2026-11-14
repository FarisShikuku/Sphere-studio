#include "BoxVanVariants.h"

namespace sphere::trailers::boxvan {
const std::vector<BoxVanVariantConfig>& BoxVanVariants::GetAll() {
    static const std::vector<BoxVanVariantConfig> kVariants = {
        {TrailerID::DRY_VAN_53FT, "53ft Dry Freight Box Van", 6600.f, 29000.f, Vec3(16.15f, 2.59f, 4.11f), false, true, Vec4(0.92f, 0.93f, 0.95f, 1.f), Vec4(0.14f, 0.36f, 0.68f, 1.f), "53FT DRY VAN"},
        {TrailerID::BOX_REEFER, "Refrigerated Reefer Van", 7800.f, 27500.f, Vec3(16.15f, 2.59f, 4.11f), true, true, Vec4(0.95f, 0.96f, 0.98f, 1.f), Vec4(0.06f, 0.56f, 0.85f, 1.f), "COLD CHAIN -25C"},
        {TrailerID::BOX_PARCEL_VAN, "Double-Stack Parcel Express Van", 5800.f, 24000.f, Vec3(13.60f, 2.55f, 4.00f), false, true, Vec4(0.96f, 0.72f, 0.12f, 1.f), Vec4(0.75f, 0.12f, 0.12f, 1.f), "EXPRESS PARCEL"},
        {TrailerID::BOX_MOVING_VAN, "Drop-Frame Household Moving Van", 6900.f, 23000.f, Vec3(14.60f, 2.55f, 4.10f), false, false, Vec4(0.90f, 0.91f, 0.93f, 1.f), Vec4(0.16f, 0.52f, 0.34f, 1.f), "TRANSIT MOVERS"},
        {TrailerID::BOX_DOUBLE_DECK, "Double-Deck High-Cube Van", 8300.f, 28000.f, Vec3(13.60f, 2.55f, 4.35f), false, true, Vec4(0.15f, 0.24f, 0.38f, 1.f), Vec4(0.92f, 0.94f, 0.96f, 1.f), "DOUBLE-DECK 4.35M"}
    };
    return kVariants;
}
const BoxVanVariantConfig* BoxVanVariants::FindVariant(TrailerID id) {
    for (const auto& v : GetAll()) if (v.id == id) return &v;
    return nullptr;
}
TrailerDef BoxVanVariants::CreateTrailerDef(const BoxVanVariantConfig& cfg) {
    TrailerDef d;
    d.id = cfg.id; d.name = cfg.name; d.category = "Box Vans";
    d.hitch_type = HitchType::FIFTH_WHEEL_KINGPIN;
    d.empty_weight_kg = cfg.emptyWeightKg; d.max_payload_kg = cfg.maxPayloadKg;
    d.dimensions = cfg.dims; d.volume_m3 = cfg.dims.x * cfg.dims.y * (cfg.dims.z - 1.3f);
    d.deck_height_m = 1.25f; d.axle_count = (cfg.id == TrailerID::BOX_DOUBLE_DECK) ? 3 : 2;
    d.base_color = cfg.baseColor; d.accent_color = cfg.accentColor;
    d.roughness = 0.38f; d.metallic = 0.25f; d.label_text = cfg.labelText;
    return d;
}
} // namespace sphere::trailers::boxvan
