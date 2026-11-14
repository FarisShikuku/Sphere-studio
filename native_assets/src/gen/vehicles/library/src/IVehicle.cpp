#include "IVehicle.h"
#include <algorithm>

namespace sphere::vehicles {

void IVehicle::SetLivery(LiveryID livery) {
    VehicleDef def = GetDef();
    def.livery = livery;
    switch (livery) {
        case LiveryID::FLEET_CORPORATE:
            def.base_color = Vec4(0.14f, 0.34f, 0.62f, 1.0f);
            def.accent_color = Vec4(0.95f, 0.60f, 0.08f, 1.0f);
            def.roughness = 0.34f;
            def.metallic = 0.35f;
            break;
        case LiveryID::NAIROBI_NGANYA:
            def.base_color = Vec4(0.10f, 0.12f, 0.18f, 1.0f);
            def.accent_color = Vec4(0.98f, 0.18f, 0.56f, 1.0f);
            def.roughness = 0.24f;
            def.metallic = 0.55f;
            break;
        case LiveryID::TRANSIT_MUNICIPAL:
            def.base_color = Vec4(0.10f, 0.48f, 0.36f, 1.0f);
            def.accent_color = Vec4(0.96f, 0.82f, 0.14f, 1.0f);
            def.roughness = 0.36f;
            def.metallic = 0.25f;
            break;
        case LiveryID::HIGHWAY_HAULAGE:
            def.base_color = Vec4(0.76f, 0.14f, 0.14f, 1.0f);
            def.accent_color = Vec4(0.92f, 0.94f, 0.96f, 1.0f);
            def.roughness = 0.30f;
            def.metallic = 0.48f;
            break;
        case LiveryID::INDUSTRIAL_WORKHORSE:
            def.base_color = Vec4(0.90f, 0.64f, 0.08f, 1.0f);
            def.accent_color = Vec4(0.18f, 0.20f, 0.23f, 1.0f);
            def.roughness = 0.48f;
            def.metallic = 0.22f;
            break;
        case LiveryID::LUXURY_METALLIC:
            def.base_color = Vec4(0.15f, 0.18f, 0.24f, 1.0f);
            def.accent_color = Vec4(0.84f, 0.72f, 0.44f, 1.0f);
            def.roughness = 0.18f;
            def.metallic = 0.78f;
            break;
    }
    SetDef(def);
}

void IVehicle::SetRandomLivery(uint32_t seed) {
    VehicleDef def = GetDef();
    def.seed = (seed == 0u) ? 0x9E3779B9u : seed;
    uint32_t h1 = def.seed * 1664525u + 1013904223u;
    uint32_t h2 = h1 * 1664525u + 1013904223u;
    uint32_t h3 = h2 * 1664525u + 1013904223u;

    auto palettePick = [](uint32_t h) -> Vec4 {
        const Vec4 vibrant[8] = {
            Vec4(0.95f, 0.16f, 0.48f, 1.0f), // Neon Magenta
            Vec4(0.08f, 0.72f, 0.88f, 1.0f), // Electric Cyan
            Vec4(0.98f, 0.74f, 0.08f, 1.0f), // Kenyan Gold
            Vec4(0.18f, 0.82f, 0.36f, 1.0f), // Lime Green
            Vec4(0.94f, 0.28f, 0.12f, 1.0f), // Vermilion Orange
            Vec4(0.56f, 0.22f, 0.92f, 1.0f), // Royal Violet
            Vec4(0.12f, 0.36f, 0.90f, 1.0f), // Cobalt Blue
            Vec4(0.94f, 0.95f, 0.98f, 1.0f)  // Gloss Pearl
        };
        return vibrant[h & 7u];
    };

    def.base_color = palettePick(h1);
    def.accent_color = palettePick(h2 ^ (h3 >> 3));
    if (std::abs(def.base_color.x - def.accent_color.x) +
        std::abs(def.base_color.y - def.accent_color.y) +
        std::abs(def.base_color.z - def.accent_color.z) < 0.35f) {
        def.accent_color = palettePick((h2 + 3u));
    }
    def.livery = LiveryID::NAIROBI_NGANYA;
    SetDef(def);
}

bool IVehicle::CanHitchTrailer() const {
    return GetDef().fifth_wheel_offset_m > 0.1f;
}

bool IVehicle::Hitch(std::shared_ptr<trailers::ITrailer> trailer) {
    if (!CanHitchTrailer()) {
        m_HitchedTrailer.reset();
        return false;
    }
    m_HitchedTrailer = std::move(trailer);
    return m_HitchedTrailer != nullptr;
}

void IVehicle::Unhitch() {
    m_HitchedTrailer.reset();
}

float IVehicle::GetTotalMassKg() const {
    const auto& def = GetDef();
    float mass = def.kerb_weight_kg;
    if (def.passenger_capacity > 2) {
        mass += static_cast<float>(def.passenger_capacity) * 72.0f;
    }
    if (m_HitchedTrailer) {
        mass += m_HitchedTrailer->GetTotalMassKg();
    }
    return mass;
}

} // namespace sphere::vehicles
