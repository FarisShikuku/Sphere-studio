#include "LoggingTrailer.h"
#include "../../running_gear/include/RunningGear.h"

namespace sphere::trailers {

LoggingTrailer::LoggingTrailer(TrailerID variantId) {
    const auto* cfg = logging::LoggingVariants::FindVariant(variantId);
    if (cfg) m_Def = logging::LoggingVariants::CreateTrailerDef(*cfg);
    else { m_Def.id = variantId; m_Def.name = GetTrailerName(variantId); m_Def.category = "Logging"; }
}

std::shared_ptr<Mesh> LoggingTrailer::GenerateMesh(uint32_t /*lod*/) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float L = m_Def.dimensions.x, W = m_Def.dimensions.y, H = m_Def.dimensions.z;
    float hl = L * 0.5f, hw = W * 0.5f;
    float y = m_Def.deck_height_m;

    // Central reach pole / heavy twin-spine rails
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, y - 0.28f, -0.35f), Vec3(hl, y, 0.35f), Vec4(0.18f, 0.20f, 0.23f, 1.f));
    // Headache protective cab guard
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, y, -hw), Vec3(-hl + 0.14f, H, hw), m_Def.base_color);

    // Bolsters & tapered vertical bunk stakes
    int bunks = (m_Def.id == TrailerID::LOGGING_POLE_TRAILER) ? 2 : 4;
    for (int b = 0; b < bunks; ++b) {
        float bx = -hl * 0.65f + (static_cast<float>(b) / std::max(1, bunks - 1)) * (L * 0.75f);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(bx - 0.14f, y, -hw), Vec3(bx + 0.14f, y + 0.22f, hw), m_Def.base_color);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(bx - 0.08f, y + 0.22f, -hw), Vec3(bx + 0.08f, H - 0.15f, -hw + 0.14f), m_Def.accent_color);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(bx - 0.08f, y + 0.22f, hw - 0.14f), Vec3(bx + 0.08f, H - 0.15f, hw), m_Def.accent_color);
    }

    // Hydraulic log lift crane at rear
    if (m_Def.id != TrailerID::LOGGING_POLE_TRAILER) {
        gear::RunningGear::AppendBox(mesh.get(), Vec3(hl - 0.8f, y, -0.22f), Vec3(hl - 0.25f, y + 2.1f, 0.22f), m_Def.base_color);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(hl - 2.3f, y + 1.9f, -0.14f), Vec3(hl - 0.25f, y + 2.25f, 0.14f), m_Def.base_color);
    }

    auto gearCfg = gear::RunningGearVariants::FromTrailerDef(m_Def);
    gear::RunningGear::AppendMesh(mesh.get(), gearCfg, hl * 0.48f, y - 0.28f, -hl + 2.4f, m_Def.landing_gear_extended);

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

} // namespace sphere::trailers
