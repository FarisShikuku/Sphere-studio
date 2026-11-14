#include "TankerTrailer.h"
#include "../../running_gear/include/RunningGear.h"
#include <algorithm>
#include <cmath>
#include <utility>

namespace sphere::trailers {

TankerTrailer::TankerTrailer(TrailerID variantId) {
    const auto* cfg = tanker::TankerVariants::FindVariant(variantId);
    if (cfg) m_Def = tanker::TankerVariants::CreateTrailerDef(*cfg);
    else { m_Def.id = variantId; m_Def.name = GetTrailerName(variantId); m_Def.category = "Tankers"; }
}

std::shared_ptr<Mesh> TankerTrailer::GenerateMesh(uint32_t lod) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float L = m_Def.dimensions.x;
    float W = m_Def.dimensions.y;
    float hl = L * 0.5f;
    float chassisY = 1.15f;
    float radZ = W * 0.44f;
    float radY = (m_Def.id == TrailerID::TANKER_FUEL_MULTI_COMPARTMENT) ? radZ * 0.78f : radZ;
    float centerY = chassisY + radY + 0.10f;

    float capDepth = std::min(L * 0.02f, std::max(radY, radZ) * 0.11f);
    float xStart = -hl * 0.92f;
    float xEnd = hl * 0.92f;
    float bodyX0 = xStart + capDepth;
    float bodyX1 = xEnd - capDepth;
    int bodyRings = (lod == 0) ? 10 : 6;
    int thetaSegs = (lod == 0) ? 28 : 16;

    std::vector<std::pair<float, float>> stations;
    stations.push_back({xStart, 0.58f});
    stations.push_back({xStart + capDepth * 0.28f, 0.84f});
    stations.push_back({xStart + capDepth * 0.68f, 0.97f});
    for (int ix = 0; ix <= bodyRings; ++ix) {
        float t = static_cast<float>(ix) / static_cast<float>(bodyRings);
        stations.push_back({bodyX0 + t * (bodyX1 - bodyX0), 1.0f});
    }
    stations.push_back({xEnd - capDepth * 0.68f, 0.97f});
    stations.push_back({xEnd - capDepth * 0.28f, 0.84f});
    stations.push_back({xEnd, 0.58f});

    uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
    // Exterior cylindrical shell with almost-flat dished bulkheads (no internal baffles)
    for (size_t ix = 0; ix < stations.size(); ++ix) {
        float x = stations[ix].first;
        float profile = stations[ix].second;
        float u = (x - xStart) / std::max(0.01f, xEnd - xStart);
        for (int s = 0; s <= thetaSegs; ++s) {
            float ang = (static_cast<float>(s) / static_cast<float>(thetaSegs)) * 6.2831853f;
            Vertex v;
            v.position = Vec3(x, centerY + radY * profile * std::sin(ang), radZ * profile * std::cos(ang));
            v.uv[0] = u;
            v.uv[1] = static_cast<float>(s) / static_cast<float>(thetaSegs);
            v.color = m_Def.base_color;
            mesh->vertices.push_back(v);
        }
    }
    for (size_t ix = 0; ix + 1 < stations.size(); ++ix) {
        for (int s = 0; s < thetaSegs; ++s) {
            uint32_t r0 = base + static_cast<uint32_t>(ix) * (thetaSegs + 1);
            uint32_t r1 = base + static_cast<uint32_t>(ix + 1) * (thetaSegs + 1);
            mesh->indices.push_back(r0 + s); mesh->indices.push_back(r1 + s); mesh->indices.push_back(r0 + s + 1);
            mesh->indices.push_back(r0 + s + 1); mesh->indices.push_back(r1 + s); mesh->indices.push_back(r1 + s + 1);
        }
    }
    // Closed flat front & rear bulkhead end discs
    Vertex vf{}; vf.position = Vec3(xStart, centerY, 0.0f); vf.uv[0] = 0.0f; vf.uv[1] = 0.5f; vf.color = m_Def.base_color;
    Vertex vb{}; vb.position = Vec3(xEnd, centerY, 0.0f); vb.uv[0] = 1.0f; vb.uv[1] = 0.5f; vb.color = m_Def.base_color;
    uint32_t frontCenterIdx = static_cast<uint32_t>(mesh->vertices.size());
    mesh->vertices.push_back(vf);
    uint32_t backCenterIdx = static_cast<uint32_t>(mesh->vertices.size());
    mesh->vertices.push_back(vb);
    uint32_t backRingBase = base + static_cast<uint32_t>(stations.size() - 1) * (thetaSegs + 1);
    for (int s = 0; s < thetaSegs; ++s) {
        mesh->indices.push_back(frontCenterIdx); mesh->indices.push_back(base + s); mesh->indices.push_back(base + s + 1);
        mesh->indices.push_back(backCenterIdx); mesh->indices.push_back(backRingBase + s + 1); mesh->indices.push_back(backRingBase + s);
    }

    // Subframe rails, top catwalk + safety rail, and discharge valve manifold
    Vec4 steelCol(0.18f, 0.20f, 0.23f, 1.0f);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, chassisY - 0.18f, -0.52f), Vec3(hl, chassisY, -0.38f), steelCol);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, chassisY - 0.18f, 0.38f), Vec3(hl, chassisY, 0.52f), steelCol);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl * 0.7f, centerY + radY, -0.32f), Vec3(hl * 0.75f, centerY + radY + 0.08f, 0.32f), m_Def.accent_color);
    gear::RunningGear::AppendBox(mesh.get(), Vec3(-0.9f, chassisY - 0.25f, -0.85f), Vec3(0.9f, chassisY - 0.02f, 0.85f), m_Def.accent_color);

    auto gearCfg = gear::RunningGearVariants::FromTrailerDef(m_Def);
    gear::RunningGear::AppendMesh(mesh.get(), gearCfg, hl * 0.45f, chassisY - 0.18f, -hl + 2.6f, m_Def.landing_gear_extended);

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

bool TankerTrailer::GenerateTextures(VkDevice, VkQueue, VkCommandPool) {
    m_IsGenerated = true;
    return true;
}

} // namespace sphere::trailers
