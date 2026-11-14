#include "FlatbedVariants.h"

namespace sphere::trailers::flatbed {

const std::vector<FlatbedVariantConfig>& FlatbedVariants::GetAll() {
    static const std::vector<FlatbedVariantConfig> kVariants = {
        {TrailerID::FLATBED_STD, "Standard 40ft Flatbed", 6200.f, 32000.f, Vec3(13.6f, 2.55f, 1.55f), 1.35f, 3, false, Vec4(0.22f, 0.26f, 0.31f, 1.f), Vec4(0.68f, 0.50f, 0.30f, 1.f), "40FT FLATBED"},
        {TrailerID::FLATBED_EXTENDABLE, "Extendable Telescopic Flatbed", 7800.f, 36000.f, Vec3(16.5f, 2.55f, 1.55f), 1.35f, 3, false, Vec4(0.75f, 0.19f, 0.15f, 1.f), Vec4(0.65f, 0.48f, 0.28f, 1.f), "EXTENDABLE 16.5M"},
        {TrailerID::FLATBED_STEP_DECK, "Step-Deck Drop Frame Flatbed", 6900.f, 34000.f, Vec3(13.6f, 2.55f, 1.55f), 0.98f, 3, true, Vec4(0.18f, 0.34f, 0.56f, 1.f), Vec4(0.66f, 0.49f, 0.30f, 1.f), "STEP-DECK"},
        {TrailerID::FLATBED_DROP_DECK, "Deep Drop-Deck Flatbed", 7400.f, 38000.f, Vec3(14.2f, 2.55f, 1.55f), 0.78f, 3, true, Vec4(0.16f, 0.18f, 0.21f, 1.f), Vec4(0.95f, 0.65f, 0.10f, 1.f), "DROP-DECK"},
        {TrailerID::FLATBED_SIDE_LOADER, "Self-Loading Crane Sideloader", 9400.f, 30000.f, Vec3(13.8f, 2.55f, 2.95f), 1.32f, 3, true, Vec4(0.88f, 0.52f, 0.08f, 1.f), Vec4(0.16f, 0.18f, 0.21f, 1.f), "SIDELOADER 36T"}
    };
    return kVariants;
}

const FlatbedVariantConfig* FlatbedVariants::FindVariant(TrailerID id) {
    for (const auto& v : GetAll()) {
        if (v.id == id) return &v;
    }
    return nullptr;
}

TrailerDef FlatbedVariants::CreateTrailerDef(const FlatbedVariantConfig& cfg) {
    TrailerDef d;
    d.id = cfg.id;
    d.name = cfg.name;
    d.category = "Flatbeds";
    d.hitch_type = HitchType::FIFTH_WHEEL_KINGPIN;
    d.empty_weight_kg = cfg.emptyWeightKg;
    d.max_payload_kg = cfg.maxPayloadKg;
    d.dimensions = cfg.dims;
    d.volume_m3 = cfg.dims.x * cfg.dims.y * 2.4f;
    d.deck_height_m = cfg.deckHeightM;
    d.axle_count = cfg.axleCount;
    d.has_lift_axle = true;
    d.base_color = cfg.baseColor;
    d.accent_color = cfg.accentColor;
    d.label_text = cfg.labelText;
    d.metallic = 0.65f;
    d.roughness = 0.48f;
    return d;
}

} // namespace sphere::trailers::flatbed
