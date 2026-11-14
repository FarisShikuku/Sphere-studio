#include "BoxVanTrailer.h"
#include "../../running_gear/include/RunningGear.h"

namespace sphere::trailers {

BoxVanTrailer::BoxVanTrailer(TrailerID variantId) {
    const auto* cfg = boxvan::BoxVanVariants::FindVariant(variantId);
    if (cfg) m_Def = boxvan::BoxVanVariants::CreateTrailerDef(*cfg);
    else { m_Def.id = variantId; m_Def.name = GetTrailerName(variantId); m_Def.category = "Box Vans"; }
}

std::shared_ptr<Mesh> BoxVanTrailer::GenerateMesh(uint32_t /*lod*/) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float L = m_Def.dimensions.x, W = m_Def.dimensions.y, H = m_Def.dimensions.z;
    float hl = L * 0.5f, hw = W * 0.5f;
    float floorY = m_Def.deck_height_m;
    float roofY = H;

    // Main insulated/dry freight cargo box
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, floorY, -hw), Vec3(hl, roofY, hw), m_Def.base_color);
    // Corner aluminum extrusion trim & rear roll-up door frame
    gear::RunningGear::AppendBox(mesh.get(), Vec3(hl - 0.08f, floorY, -hw - 0.01f), Vec3(hl + 0.02f, roofY, hw + 0.01f), m_Def.accent_color);
    // Aerodynamic side skirts
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl * 0.35f, floorY - 0.55f, -hw), Vec3(hl * 0.22f, floorY, -hw + 0.05f), m_Def.accent_color);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl * 0.35f, floorY - 0.55f, hw - 0.05f), Vec3(hl * 0.22f, floorY, hw), m_Def.accent_color);
    // Front reefer condenser unit if BOX_REEFER
    if (m_Def.id == TrailerID::BOX_REEFER) {
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl - 0.52f, floorY + 0.75f, -hw * 0.65f), Vec3(-hl, roofY - 0.12f, hw * 0.65f), Vec4(0.86f, 0.89f, 0.93f, 1.f));
    }

    auto gearCfg = gear::RunningGearVariants::FromTrailerDef(m_Def);
    gear::RunningGear::AppendMesh(mesh.get(), gearCfg, hl * 0.55f, floorY - 0.14f, -hl + 2.8f, m_Def.landing_gear_extended);

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

} // namespace sphere::trailers
