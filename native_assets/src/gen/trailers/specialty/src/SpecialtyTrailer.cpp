#include "SpecialtyTrailer.h"
#include "../../running_gear/include/RunningGear.h"
#include <algorithm>
#include <cmath>
#include <utility>

namespace sphere::trailers {

SpecialtyTrailer::SpecialtyTrailer(TrailerID variantId) {
    const auto* cfg = specialty::SpecialtyVariants::FindVariant(variantId);
    if (cfg) m_Def = specialty::SpecialtyVariants::CreateTrailerDef(*cfg);
    else { m_Def.id = variantId; m_Def.name = GetTrailerName(variantId); m_Def.category = "Specialty"; }
}

std::shared_ptr<Mesh> SpecialtyTrailer::GenerateMesh(uint32_t lod) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float L = m_Def.dimensions.x, W = m_Def.dimensions.y, H = m_Def.dimensions.z;
    float hl = L * 0.5f, hw = W * 0.5f;
    float y = m_Def.deck_height_m;

    if (m_Def.id == TrailerID::SPECIALTY_MILITARY_TRANSPORT) {
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, y - 0.18f, -hw), Vec3(hl, y + 0.08f, hw), m_Def.base_color);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(hl - 0.8f, y, -hw), Vec3(hl + 0.6f, y + 1.25f, hw), m_Def.accent_color);
    } else if (m_Def.id == TrailerID::SPECIALTY_LIVESTOCK) {
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, y - 0.14f, -hw), Vec3(hl, y, hw), m_Def.accent_color);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, H - 0.12f, -hw), Vec3(hl, H, hw), m_Def.base_color);
        for (int s = 0; s < 5; ++s) {
            float sy = y + 0.25f + static_cast<float>(s) * ((H - y - 0.5f) / 4.5f);
            gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, sy, -hw), Vec3(hl, sy + 0.20f, -hw + 0.06f), m_Def.base_color);
            gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, sy, hw - 0.06f), Vec3(hl, sy + 0.20f, hw), m_Def.base_color);
        }
    } else if (m_Def.id == TrailerID::SPECIALTY_WASTE_TANKER) {
        // Exterior cylindrical vacuum waste barrel with almost-flat dished bulkheads (no internal geometry)
        float chassisY = 1.15f;
        float rad = W * 0.43f;
        float centerY = chassisY + rad + 0.10f;
        float capDepth = std::min(L * 0.02f, rad * 0.11f);
        float xStart = -hl * 0.90f;
        float xEnd = hl * 0.90f;
        float bodyX0 = xStart + capDepth;
        float bodyX1 = xEnd - capDepth;
        int bodyRings = (lod == 0) ? 8 : 5;
        int thetaSegs = (lod == 0) ? 28 : 16;

        std::vector<std::pair<float, float>> stations;
        stations.push_back({xStart, 0.58f});
        stations.push_back({xStart + capDepth * 0.30f, 0.85f});
        stations.push_back({xStart + capDepth * 0.70f, 0.97f});
        for (int ix = 0; ix <= bodyRings; ++ix) {
            float t = static_cast<float>(ix) / static_cast<float>(bodyRings);
            stations.push_back({bodyX0 + t * (bodyX1 - bodyX0), 1.0f});
        }
        stations.push_back({xEnd - capDepth * 0.70f, 0.97f});
        stations.push_back({xEnd - capDepth * 0.30f, 0.85f});
        stations.push_back({xEnd, 0.58f});

        uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
        for (size_t ix = 0; ix < stations.size(); ++ix) {
            float sx = stations[ix].first;
            float profile = stations[ix].second;
            for (int s = 0; s <= thetaSegs; ++s) {
                float ang = (static_cast<float>(s) / static_cast<float>(thetaSegs)) * 6.2831853f;
                Vertex v;
                v.position = Vec3(sx, centerY + rad * profile * std::sin(ang), rad * profile * std::cos(ang));
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
        Vertex vf{}; vf.position = Vec3(xStart, centerY, 0.0f); vf.color = m_Def.base_color;
        Vertex vb{}; vb.position = Vec3(xEnd, centerY, 0.0f); vb.color = m_Def.base_color;
        uint32_t frontCenter = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(vf);
        uint32_t backCenter = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(vb);
        uint32_t backRing = base + static_cast<uint32_t>(stations.size() - 1) * (thetaSegs + 1);
        for (int s = 0; s < thetaSegs; ++s) {
            mesh->indices.push_back(frontCenter); mesh->indices.push_back(base + s); mesh->indices.push_back(base + s + 1);
            mesh->indices.push_back(backCenter); mesh->indices.push_back(backRing + s + 1); mesh->indices.push_back(backRing + s);
        }
        Vec4 steelCol(0.18f, 0.20f, 0.23f, 1.0f);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, chassisY - 0.18f, -0.50f), Vec3(hl, chassisY, -0.36f), steelCol);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, chassisY - 0.18f, 0.36f), Vec3(hl, chassisY, 0.50f), steelCol);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-0.9f, chassisY - 0.25f, -0.85f), Vec3(0.9f, chassisY, 0.85f), m_Def.accent_color);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl * 0.4f, centerY + rad, -0.28f), Vec3(hl * 0.4f, centerY + rad + 0.12f, 0.28f), m_Def.accent_color);
        y = chassisY;
    } else {
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl, y, -hw), Vec3(hl, H, hw), m_Def.base_color);
        gear::RunningGear::AppendBox(mesh.get(), Vec3(-hl + 0.2f, y + 0.4f, -hw - 0.04f), Vec3(hl - 0.2f, H - 0.4f, hw + 0.04f), m_Def.accent_color);
    }

    auto gearCfg = gear::RunningGearVariants::FromTrailerDef(m_Def);
    gear::RunningGear::AppendMesh(mesh.get(), gearCfg, hl * 0.48f, y - 0.18f, -hl + 2.6f, m_Def.landing_gear_extended);

    mesh->CalculateBounds();
    m_Bounds = mesh->bounds;
    m_CachedMesh = mesh;
    return mesh;
}

} // namespace sphere::trailers
