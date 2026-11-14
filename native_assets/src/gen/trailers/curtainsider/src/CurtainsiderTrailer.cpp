#include "CurtainsiderTrailer.h"
#include "../../running_gear/include/RunningGear.h"
#include <algorithm>

namespace sphere::trailers {

CurtainsiderTrailer::CurtainsiderTrailer(TrailerID variantId) {
    const auto* cfg = curtainsider::CurtainsiderVariants::FindVariant(variantId);
    if (cfg) m_Def = curtainsider::CurtainsiderVariants::CreateTrailerDef(*cfg);
    else { m_Def.id = variantId; m_Def.name = GetTrailerName(variantId); m_Def.category = "Curtainsiders"; }
}

std::shared_ptr<Mesh> CurtainsiderTrailer::GenerateMesh(uint32_t /*lod*/) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float L = m_Def.dimensions.x, W = m_Def.dimensions.y, H = m_Def.dimensions.z;
    float hl = L * 0.5f, hw = W * 0.5f;
    float floorY = m_Def.deck_height_m;
    float roofY = H;
    Vec4 aluminumCol(0.76f, 0.79f, 0.83f, 1.0f);
    Vec4 buckleCol(0.84f, 0.86f, 0.89f, 1.0f);

    // Deck floor & roof canopy
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, floorY - 0.14f, -hw), Vec3(hl, floorY, hw), Vec4(0.24f, 0.26f, 0.29f, 1.f));
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, roofY - 0.10f, -hw), Vec3(hl, roofY, hw), Vec4(0.94f, 0.95f, 0.96f, 1.f));
    // Front bulkhead & rear doors
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, floorY, -hw), Vec3(-hl + 0.10f, roofY, hw), aluminumCol);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(hl - 0.10f, floorY, -hw), Vec3(hl, roofY, hw), aluminumCol);

    // Sliding stanchion posts + PVC curtains + tensioning buckles
    for (int p = 0; p < 5; ++p) {
        float px = -hl * 0.75f + (static_cast<float>(p) / 4.0f) * (L * 0.75f);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(px - 0.05f, floorY, -hw - 0.01f), Vec3(px + 0.05f, roofY, -hw + 0.04f), aluminumCol);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(px - 0.05f, floorY, hw - 0.04f), Vec3(px + 0.05f, roofY, hw + 0.01f), aluminumCol);
        // Tension strap buckles near bottom coaming
        gear::RunningGear::AppendBox(mesh.get(), Vec3(px - 0.03f, floorY - 0.06f, -hw - 0.03f), Vec3(px + 0.03f, floorY + 0.08f, -hw), buckleCol);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(px - 0.03f, floorY - 0.06f, hw), Vec3(px + 0.03f, floorY + 0.08f, hw + 0.03f), buckleCol);
    }

    // Side curtains (opaque by default unless opacity_bias < 0.99f; slide forward when curtain_open_ratio > 0)
    Vec4 curtainCol = m_Def.base_color;
    curtainCol.w = (m_Def.opacity_bias >= 0.99f) ? 1.0f : std::clamp(m_Def.opacity_bias, 0.15f, 1.0f);
    float curtainSpan = (L - 0.20f) * std::clamp(1.0f - m_Def.curtain_open_ratio * 0.85f, 0.12f, 1.0f);
    float curtainStartX = -hl + 0.10f;
    float curtainEndX = curtainStartX + curtainSpan;
    gear::RunningGear::AppendBox(mesh.get(), Vec3(curtainStartX, floorY + 0.04f, -hw), Vec3(curtainEndX, roofY - 0.10f, -hw + 0.03f), curtainCol);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(curtainStartX, floorY + 0.04f, hw - 0.03f), Vec3(curtainEndX, roofY - 0.10f, hw), curtainCol);

    auto gearCfg = gear::RunningGearVariants::FromTrailerDef(m_Def);
    gear::RunningGear::AppendMesh(mesh.get(), gearCfg, hl * 0.48f, floorY - 0.14f, -hl + 2.8f, m_Def.landing_gear_extended);

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

} // namespace sphere::trailers
