#include "HeavyHaulageTrailer.h"
#include "../../running_gear/include/RunningGear.h"

namespace sphere::trailers {

HeavyHaulageTrailer::HeavyHaulageTrailer(TrailerID variantId) {
    const auto* cfg = heavy::HeavyHaulageVariants::FindVariant(variantId);
    if (cfg) m_Def = heavy::HeavyHaulageVariants::CreateTrailerDef(*cfg);
    else { m_Def.id = variantId; m_Def.name = GetTrailerName(variantId); m_Def.category = "Heavy Haulage"; }
}

std::shared_ptr<Mesh> HeavyHaulageTrailer::GenerateMesh(uint32_t /*lod*/) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float L = m_Def.dimensions.x, W = m_Def.dimensions.y;
    float hl = L * 0.5f, hw = W * 0.5f;
    float wellY = m_Def.deck_height_m;

    // Detachable hydraulic swan-neck (upper) + deep well I-beams + rear bridge over axles
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, 1.38f, -0.68f), Vec3(-hl + 3.2f, 1.72f, 0.68f), m_Def.base_color);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl + 2.8f, wellY, -0.68f), Vec3(-hl + 3.4f, 1.65f, 0.68f), m_Def.accent_color);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl + 3.2f, wellY - 0.15f, -hw), Vec3(hl * 0.25f, wellY + 0.08f, hw), m_Def.base_color);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(hl * 0.25f, 0.98f, -hw), Vec3(hl, 1.18f, hw), m_Def.base_color);

    auto gearCfg = gear::RunningGearVariants::FromTrailerDef(m_Def);
    gearCfg.axle_spacing_m = 1.18f;
    gear::RunningGear::AppendMesh(mesh.get(), gearCfg, hl * 0.62f, 0.98f, -hl + 3.4f, m_Def.landing_gear_extended);

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

} // namespace sphere::trailers
