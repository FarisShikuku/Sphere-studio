#include "FlatbedTrailer.h"
#include "../../running_gear/include/RunningGear.h"
#include <algorithm>

namespace sphere::trailers {

FlatbedTrailer::FlatbedTrailer(TrailerID variantId) {
    const auto* cfg = flatbed::FlatbedVariants::FindVariant(variantId);
    if (cfg) {
        m_Def = flatbed::FlatbedVariants::CreateTrailerDef(*cfg);
    } else {
        m_Def.id = variantId;
        m_Def.name = GetTrailerName(variantId);
        m_Def.category = "Flatbeds";
    }
}

std::shared_ptr<Mesh> FlatbedTrailer::GenerateMesh(uint32_t /*lod*/) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float L = m_Def.dimensions.x;
    float W = m_Def.dimensions.y;
    float deckY = m_Def.deck_height_m;
    float hl = L * 0.5f;
    float hw = W * 0.5f;

    // Main twin I-beam longitudinal rails
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, deckY - 0.34f, -hw * 0.44f), Vec3(hl, deckY - 0.04f, -hw * 0.34f), m_Def.base_color);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, deckY - 0.34f, hw * 0.34f), Vec3(hl, deckY - 0.04f, hw * 0.44f), m_Def.base_color);

    // Hardwood deck planking + steel perimeter coaming rails
    if (m_Def.id == TrailerID::FLATBED_STEP_DECK || m_Def.id == TrailerID::FLATBED_DROP_DECK) {
        float upperDeckY = 1.38f;
        float stepX = -hl + L * 0.28f;
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, upperDeckY - 0.05f, -hw), Vec3(stepX, upperDeckY, hw), m_Def.accent_color);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(stepX, deckY - 0.05f, -hw), Vec3(hl, deckY, hw), m_Def.accent_color);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(stepX - 0.14f, deckY - 0.05f, -hw), Vec3(stepX, upperDeckY, hw), m_Def.base_color);
    } else {
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, deckY - 0.05f, -hw), Vec3(hl, deckY, hw), m_Def.accent_color);
    }

    // Steel headboard bulkhead
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, deckY, -hw), Vec3(-hl + 0.12f, deckY + 1.35f, hw), m_Def.base_color);

    // Cross-members & ISO twist-lock pockets
    for (int i = 0; i <= 8; ++i) {
        float x = -hl + (static_cast<float>(i) / 8.0f) * L;
        gear::RunningGear::AppendBox(mesh.get(), Vec3(x - 0.05f, deckY - 0.18f, -hw), Vec3(x + 0.05f, deckY - 0.05f, hw), m_Def.base_color);
    }

    // Sideloader crane towers if FLATBED_SIDE_LOADER
    if (m_Def.id == TrailerID::FLATBED_SIDE_LOADER) {
        Vec4 craneYellow(0.92f, 0.62f, 0.08f, 1.0f);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl + 0.5f, deckY, -hw - 0.12f), Vec3(-hl + 1.25f, deckY + 1.65f, -hw * 0.45f), craneYellow);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(hl - 1.25f, deckY, -hw - 0.12f), Vec3(hl - 0.5f, deckY + 1.65f, -hw * 0.45f), craneYellow);
    }

    // Attach shared RunningGear
    auto gearCfg = gear::RunningGearVariants::FromTrailerDef(m_Def);
    gear::RunningGear::AppendMesh(mesh.get(), gearCfg, hl * 0.48f, deckY - 0.34f, -hl + 2.8f, m_Def.landing_gear_extended);

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

bool FlatbedTrailer::GenerateTextures(VkDevice, VkQueue, VkCommandPool) {
    m_IsGenerated = true;
    return true;
}

} // namespace sphere::trailers
