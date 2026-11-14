#include "LoggingVariants.h"

namespace sphere::trailers::logging {
const std::vector<LoggingVariantConfig>& LoggingVariants::GetAll() {
    static const std::vector<LoggingVariantConfig> kVariants = {
        {TrailerID::LOGGING_POLE_TRAILER, "Steerable Forestry Pole Trailer", 4800.f, 36000.f, Vec3(13.8f, 2.55f, 3.85f), 2, false, Vec4(0.84f, 0.36f, 0.08f, 1.f), "TIMBER POLE"},
        {TrailerID::LOGGING_BUNK_TRAILER, "Heavy Bolster Logging Bunk Trailer", 6400.f, 38000.f, Vec3(13.6f, 2.55f, 3.95f), 4, true, Vec4(0.75f, 0.16f, 0.12f, 1.f), "4-BUNK LOGGING"},
        {TrailerID::LOGGING_EXTENDABLE_LOG, "Telescopic Long-Log Timber Trailer", 7100.f, 40000.f, Vec3(16.2f, 2.55f, 3.95f), 5, true, Vec4(0.16f, 0.42f, 0.24f, 1.f), "LONG-LOG FORESTRY"}
    };
    return kVariants;
}
const LoggingVariantConfig* LoggingVariants::FindVariant(TrailerID id) {
    for (const auto& v : GetAll()) if (v.id == id) return &v;
    return nullptr;
}
TrailerDef LoggingVariants::CreateTrailerDef(const LoggingVariantConfig& cfg) {
    TrailerDef d;
    d.id = cfg.id; d.name = cfg.name; d.category = "Logging";
    d.hitch_type = HitchType::FIFTH_WHEEL_KINGPIN;
    d.empty_weight_kg = cfg.emptyWeightKg; d.max_payload_kg = cfg.maxPayloadKg;
    d.dimensions = cfg.dims; d.volume_m3 = 68.0f;
    d.deck_height_m = 1.28f; d.axle_count = 3;
    d.has_steerable_axle = true;
    d.suspension_type = SuspensionType::LEAF_SPRING;
    d.base_color = cfg.baseColor; d.accent_color = Vec4(0.95f, 0.72f, 0.12f, 1.f);
    d.metallic = 0.65f; d.roughness = 0.52f; d.label_text = cfg.labelText;
    return d;
}
} // namespace sphere::trailers::logging
