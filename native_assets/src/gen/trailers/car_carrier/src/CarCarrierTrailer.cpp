#include "CarCarrierTrailer.h"
#include "../../running_gear/include/RunningGear.h"

namespace sphere::trailers {

CarCarrierTrailer::CarCarrierTrailer(TrailerID variantId) {
    const auto* cfg = carcarrier::CarCarrierVariants::FindVariant(variantId);
    if (cfg) m_Def = carcarrier::CarCarrierVariants::CreateTrailerDef(*cfg);
    else { m_Def.id = variantId; m_Def.name = GetTrailerName(variantId); m_Def.category = "Car Carriers"; }
}

std::shared_ptr<Mesh> CarCarrierTrailer::GenerateMesh(uint32_t /*lod*/) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float L = m_Def.dimensions.x, W = m_Def.dimensions.y, H = m_Def.dimensions.z;
    float hl = L * 0.5f, hw = W * 0.5f;
    float lowerY = m_Def.deck_height_m;

    // Twin lower wheel-track channels
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, lowerY - 0.14f, -hw), Vec3(hl, lowerY, -hw * 0.36f), m_Def.base_color);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, lowerY - 0.14f, hw * 0.36f), Vec3(hl, lowerY, hw), m_Def.base_color);

    if (m_Def.id == TrailerID::CARCARRIER_ENCLOSED) {
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, lowerY, -hw), Vec3(hl, H, hw), m_Def.base_color);
    } else if (m_Def.id != TrailerID::CARCARRIER_SINGLE_DECK) {
        float upperY = lowerY + 1.75f + m_Def.hydraulic_lift_ratio * 0.65f;
        // Upper hydraulic lifting track decks + hydraulic rams
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl * 0.92f, upperY, -hw), Vec3(hl * 0.92f, upperY + 0.12f, -hw * 0.36f), m_Def.accent_color);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl * 0.92f, upperY, hw * 0.36f), Vec3(hl * 0.92f, upperY + 0.12f, hw), m_Def.accent_color);
        for (int p = 0; p < 4; ++p) {
            float px = -hl * 0.78f + static_cast<float>(p) * (L * 0.52f);
            gear::RunningGear::AppendBox(mesh.get(), Vec3(px - 0.08f, lowerY, -hw), Vec3(px + 0.08f, upperY + 0.15f, -hw + 0.14f), m_Def.base_color);
            gear::RunningGear::AppendBox(mesh.get(), Vec3(px - 0.08f, lowerY, hw - 0.14f), Vec3(px + 0.08f, upperY + 0.15f, hw), m_Def.base_color);
        }
    }

    auto gearCfg = gear::RunningGearVariants::FromTrailerDef(m_Def);
    gear::RunningGear::AppendMesh(mesh.get(), gearCfg, hl * 0.50f, lowerY - 0.14f, -hl + 2.4f, m_Def.landing_gear_extended);

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

} // namespace sphere::trailers
