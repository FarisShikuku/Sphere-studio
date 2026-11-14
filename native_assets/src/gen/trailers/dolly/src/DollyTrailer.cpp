#include "DollyTrailer.h"
#include "../../running_gear/include/RunningGear.h"

namespace sphere::trailers {

DollyTrailer::DollyTrailer(TrailerID variantId) {
    const auto* cfg = dolly::DollyVariants::FindVariant(variantId);
    if (cfg) m_Def = dolly::DollyVariants::CreateTrailerDef(*cfg);
    else { m_Def.id = variantId; m_Def.name = GetTrailerName(variantId); m_Def.category = "Dollies"; }
}

std::shared_ptr<Mesh> DollyTrailer::GenerateMesh(uint32_t /*lod*/) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float L = m_Def.dimensions.x;
    float hl = L * 0.5f;
    float y = m_Def.deck_height_m;

    // A-frame drawbar + pintle eye + fifth-wheel turntable plate
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, 0.78f, -0.10f), Vec3(0.0f, 0.92f, 0.10f), m_Def.base_color);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(0.0f, y - 0.18f, -0.82f), Vec3(hl, y, 0.82f), m_Def.base_color);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(0.25f, y, -0.52f), Vec3(hl - 0.25f, y + 0.14f, 0.52f), m_Def.accent_color);

    auto gearCfg = gear::RunningGearVariants::FromTrailerDef(m_Def);
    gear::RunningGear::AppendMesh(mesh.get(), gearCfg, hl * 0.52f, y - 0.18f, -0.2f, m_Def.landing_gear_extended);

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

} // namespace sphere::trailers
