#include "ContainersBottlesCargo.h"
#include <cmath>
#include <algorithm>

namespace sphere::cargo {

ContainersBottlesCargo::ContainersBottlesCargo(CargoID variantId) {
    const BottleVariantConfig* cfg = ContainersBottlesVariants::FindVariant(variantId);
    if (cfg) {
        m_Def = ContainersBottlesVariants::CreateCargoDef(*cfg);
    } else {
        m_Def.id = variantId;
        m_Def.name = GetCargoName(variantId);
        m_Def.category = "Containers & Bottles";
        m_Def.packingType = CargoPackingType::BOTTLE;
    }
}

std::shared_ptr<Mesh> ContainersBottlesCargo::GenerateMesh(uint32_t lod) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    uint32_t radialSegs = (lod == 0) ? 36 : 20;
    uint32_t profileSegs = 32;

    float baseRadius = m_Def.dimensions.x * 0.5f;
    float totalH = m_Def.dimensions.z;

    // C1-smooth cubic Hermite & Gaussian rib lathe profile equation r(v)
    for (uint32_t ip = 0; ip <= profileSegs; ++ip) {
        float v = static_cast<float>(ip) / static_cast<float>(profileSegs);
        float y = v * totalH;

        float r = baseRadius;
        if (m_Def.id == CargoID::TIN_CAN) {
            // Sanitary food can with 5 sinusoidal vacuum expansion beads + double-seamed top/bottom chimes
            float chime = (v < 0.04f || v > 0.96f) ? 0.0025f : 0.0f;
            float bead = (v > 0.14f && v < 0.86f) ? 0.0018f * std::sin((v - 0.14f) / 0.72f * 10.0f * 3.14159f) : 0.0f;
            r = baseRadius + chime + bead;
        } else if (m_Def.id == CargoID::JAR_GLASS) {
            // Curved shoulder (v in [0.70, 0.82]) -> neck -> wider fluted lug lid (v > 0.86)
            if (v < 0.06f) {
                r = baseRadius * (0.90f + 0.10f * (v / 0.06f));
            } else if (v > 0.70f && v <= 0.84f) {
                float t = (v - 0.70f) / 0.14f;
                float smoothT = t * t * (3.0f - 2.0f * t);
                r = baseRadius * (1.0f - 0.18f * smoothT);
            } else if (v > 0.84f) {
                r = baseRadius * 0.86f; // lug cap flange
            }
        } else {
            // Beer / Soda / Water bottles with cubic Hermite Sigmoid shoulder & waist/structural ribs
            if (v < 0.05f) {
                r = baseRadius * (0.86f + 0.14f * std::sin((v / 0.05f) * 1.57079f));
            } else if (v < 0.56f) {
                // PET structural grip ribs for water/soda
                float rib = (m_Def.id != CargoID::GLASS_BOTTLE_BEER)
                    ? (-0.06f * baseRadius * std::pow(std::sin(v * 22.0f), 8.0f))
                    : 0.0f;
                r = baseRadius + rib;
            } else if (v < 0.78f) {
                // Cubic Hermite Sigmoid shoulder: r_neck + (r_body - r_neck) * (1 - 3t^2 + 2t^3)
                float t = (v - 0.56f) / 0.22f;
                float hermite = 1.0f - (3.0f * t * t - 2.0f * t * t * t);
                r = baseRadius * (0.34f + 0.66f * hermite);
            } else if (v < 0.94f) {
                // Neck cylinder + collar flange
                float ring = (v > 0.88f && v < 0.92f) ? (baseRadius * 0.06f) : 0.0f;
                r = baseRadius * 0.34f + ring;
            } else {
                // Crown / screw cap finish
                r = baseRadius * 0.40f;
            }
        }

        for (uint32_t ir = 0; ir <= radialSegs; ++ir) {
            float u = static_cast<float>(ir) / static_cast<float>(radialSegs);
            float angle = u * 6.2831853f;
            Vertex vert;
            vert.position = Vec3(std::cos(angle) * r, y, std::sin(angle) * r);
            vert.uv[0] = u;
            vert.uv[1] = v;
            vert.color = (v > 0.92f || (v > 0.20f && v < 0.52f)) ? m_Def.label_color : m_Def.base_color;
            mesh->vertices.push_back(vert);
        }
    }

    for (uint32_t ip = 0; ip < profileSegs; ++ip) {
        for (uint32_t ir = 0; ir < radialSegs; ++ir) {
            uint32_t row0 = ip * (radialSegs + 1);
            uint32_t row1 = (ip + 1) * (radialSegs + 1);
            mesh->indices.push_back(row0 + ir);
            mesh->indices.push_back(row1 + ir);
            mesh->indices.push_back(row0 + ir + 1);
            mesh->indices.push_back(row0 + ir + 1);
            mesh->indices.push_back(row1 + ir);
            mesh->indices.push_back(row1 + ir + 1);
        }
    }

    // Closed bottom (-Y) and top (+Y) cap discs
    uint32_t botRing = 0;
    uint32_t topRing = profileSegs * (radialSegs + 1);
    Vertex bc{}; bc.position = Vec3(0.0f, 0.0f, 0.0f); bc.uv[0] = 0.5f; bc.uv[1] = 0.0f; bc.color = m_Def.base_color * 0.9f;
    Vertex tc{}; tc.position = Vec3(0.0f, totalH, 0.0f); tc.uv[0] = 0.5f; tc.uv[1] = 1.0f; tc.color = m_Def.label_color;
    uint32_t bcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(bc);
    uint32_t tcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(tc);
    for (uint32_t ir = 0; ir < radialSegs; ++ir) {
        mesh->indices.push_back(bcIdx); mesh->indices.push_back(botRing + ir); mesh->indices.push_back(botRing + ir + 1);
        mesh->indices.push_back(tcIdx); mesh->indices.push_back(topRing + ir + 1); mesh->indices.push_back(topRing + ir);
    }

    mesh->CalculateSmoothNormals();
    mesh->CalculateTangents();
    mesh->CalculateBounds();

    m_CachedMesh = mesh;
    m_Bounds = mesh->bounds;
    return mesh;
}

bool ContainersBottlesCargo::GenerateTextures(VkDevice device, VkQueue queue, VkCommandPool pool) {
    m_IsGenerated = true;
    return true;
}

} // namespace sphere::cargo
