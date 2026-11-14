#include "RecreationalTrailer.h"
#include "../../running_gear/include/RunningGear.h"

namespace sphere::trailers {

RecreationalTrailer::RecreationalTrailer(TrailerID variantId) {
    const auto* cfg = recreational::RecreationalVariants::FindVariant(variantId);
    if (cfg) m_Def = recreational::RecreationalVariants::CreateTrailerDef(*cfg);
    else { m_Def.id = variantId; m_Def.name = GetTrailerName(variantId); m_Def.category = "Recreational"; }
}

std::shared_ptr<Mesh> RecreationalTrailer::GenerateMesh(uint32_t /*lod*/) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float L = m_Def.dimensions.x, W = m_Def.dimensions.y, H = m_Def.dimensions.z;
    float hl = L * 0.5f, hw = W * 0.5f;
    float y = m_Def.deck_height_m;

    // A-frame drawbar tongue
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, 0.48f, -0.10f), Vec3(-hl + 1.5f, 0.58f, 0.10f), m_Def.base_color);

    if (m_Def.id == TrailerID::RECREATION_BOAT_TRAILER) {
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl + 1.4f, y - 0.10f, -hw * 0.6f), Vec3(hl, y, -hw * 0.45f), m_Def.base_color);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl + 1.4f, y - 0.10f, hw * 0.45f), Vec3(hl, y, hw * 0.6f), m_Def.base_color);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl + 1.5f, y, -0.12f), Vec3(-hl + 1.8f, y + 0.85f, 0.12f), m_Def.base_color); // Winch post
        for (int r = 0; r < 5; ++r) {
            float rx = -hl * 0.35f + static_cast<float>(r) * (L * 0.18f);
            gear::RunningGear::AppendBox(mesh.get(), Vec3(rx - 0.08f, y, -0.35f), Vec3(rx + 0.08f, y + 0.12f, 0.35f), m_Def.accent_color);
        }
    } else if (m_Def.id == TrailerID::RECREATION_CAMPER_TRAILER) {
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl + 1.2f, y, -hw), Vec3(hl, H, hw), m_Def.base_color);
    } else {
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl + 1.2f, y - 0.08f, -hw), Vec3(hl, y, hw), m_Def.base_color);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl + 1.2f, y, -hw), Vec3(hl, y + 0.48f, -hw + 0.06f), m_Def.base_color);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl + 1.2f, y, hw - 0.06f), Vec3(hl, y + 0.48f, hw), m_Def.base_color);
    }

    auto gearCfg = gear::RunningGearVariants::FromTrailerDef(m_Def);
    gearCfg.axle_spacing_m = 0.86f;
    gear::RunningGear::AppendMesh(mesh.get(), gearCfg, hl * 0.18f, y - 0.10f, -hl + 1.35f, m_Def.landing_gear_extended);

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

} // namespace sphere::trailers
