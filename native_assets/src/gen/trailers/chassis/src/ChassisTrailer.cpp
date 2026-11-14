#include "ChassisTrailer.h"
#include "../../running_gear/include/RunningGear.h"

namespace sphere::trailers {

ChassisTrailer::ChassisTrailer(TrailerID variantId) {
    const auto* cfg = chassis::ChassisVariants::FindVariant(variantId);
    if (cfg) m_Def = chassis::ChassisVariants::CreateTrailerDef(*cfg);
    else { m_Def.id = variantId; m_Def.name = GetTrailerName(variantId); m_Def.category = "Container Chassis"; }
}

std::shared_ptr<Mesh> ChassisTrailer::GenerateMesh(uint32_t /*lod*/) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float L = m_Def.dimensions.x, W = m_Def.dimensions.y;
    float hl = L * 0.5f, hw = W * 0.5f;
    float y = m_Def.deck_height_m;

    // Skeletal I-beam main rails + front/rear twist-lock bolsters
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, y - 0.30f, -0.52f), Vec3(hl, y, -0.36f), m_Def.base_color);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, y - 0.30f, 0.36f), Vec3(hl, y, 0.52f), m_Def.base_color);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, y - 0.18f, -hw), Vec3(-hl + 0.32f, y + 0.04f, hw), m_Def.accent_color);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(hl - 0.32f, y - 0.18f, -hw), Vec3(hl, y + 0.04f, hw), m_Def.accent_color);

    // Center cross-brace & optional hydraulic tipping ram
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-0.2f, y - 0.20f, -hw), Vec3(0.2f, y + 0.02f, hw), m_Def.base_color);
    if (m_Def.id == TrailerID::CHASSIS_TIPPING) {
        float ramTop = y + 1.1f + m_Def.hydraulic_lift_ratio * 1.45f;
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl + 0.9f, y, -0.15f), Vec3(-hl + 1.25f, ramTop, 0.15f), Vec4(0.84f, 0.86f, 0.89f, 1.f));
    }

    auto gearCfg = gear::RunningGearVariants::FromTrailerDef(m_Def);
    gear::RunningGear::AppendMesh(mesh.get(), gearCfg, hl * 0.45f, y - 0.30f, -hl + 2.2f, m_Def.landing_gear_extended);

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

} // namespace sphere::trailers
