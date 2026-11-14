#include "BulkHopperTrailer.h"
#include "../../running_gear/include/RunningGear.h"

namespace sphere::trailers {

BulkHopperTrailer::BulkHopperTrailer(TrailerID variantId) {
    const auto* cfg = hopper::BulkHopperVariants::FindVariant(variantId);
    if (cfg) m_Def = hopper::BulkHopperVariants::CreateTrailerDef(*cfg);
    else { m_Def.id = variantId; m_Def.name = GetTrailerName(variantId); m_Def.category = "Bulk Hoppers"; }
}

std::shared_ptr<Mesh> BulkHopperTrailer::GenerateMesh(uint32_t /*lod*/) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float L = m_Def.dimensions.x, W = m_Def.dimensions.y, H = m_Def.dimensions.z;
    float hl = L * 0.5f, hw = W * 0.5f;
    float y = m_Def.deck_height_m;

    // Chassis rails
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, y - 0.24f, -0.52f), Vec3(hl, y, -0.38f), Vec4(0.18f, 0.20f, 0.23f, 1.f));
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, y - 0.24f, 0.38f), Vec3(hl, y, 0.52f), Vec4(0.18f, 0.20f, 0.23f, 1.f));

    float liftOffset = ((m_Def.id == TrailerID::HOPPER_AGGREGATE_TIPPER || m_Def.id == TrailerID::HOPPER_SIDE_TIPPER)
        ? m_Def.hydraulic_lift_ratio * 1.45f : 0.0f);
    if (liftOffset > 0.02f) {
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl * 0.72f, y, -0.11f), Vec3(-hl * 0.58f, y + liftOffset + 0.55f, 0.11f), Vec4(0.82f, 0.85f, 0.88f, 1.f));
    }

    // Upper hopper tub + V-bottom discharge cones + top hatch frames
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl * 0.92f, y + 0.55f + liftOffset, -hw), Vec3(hl * 0.92f, H - 0.15f + liftOffset, hw), m_Def.base_color);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl * 0.55f, y - 0.15f + liftOffset, -0.58f), Vec3(-0.35f, y + 0.55f + liftOffset, 0.58f), m_Def.accent_color);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(0.35f, y - 0.15f + liftOffset, -0.58f), Vec3(hl * 0.55f, y + 0.55f + liftOffset, 0.58f), m_Def.accent_color);

    // Top loading hatches
    for (int h = 0; h < 3; ++h) {
        float hx = -hl * 0.55f + static_cast<float>(h) * (L * 0.28f);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(hx - 0.35f, H - 0.15f + liftOffset, -0.45f), Vec3(hx + 0.35f, H + 0.05f + liftOffset, 0.45f), m_Def.accent_color);
    }

    auto gearCfg = gear::RunningGearVariants::FromTrailerDef(m_Def);
    gear::RunningGear::AppendMesh(mesh.get(), gearCfg, hl * 0.45f, y - 0.24f, -hl + 2.2f, m_Def.landing_gear_extended);

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

} // namespace sphere::trailers
