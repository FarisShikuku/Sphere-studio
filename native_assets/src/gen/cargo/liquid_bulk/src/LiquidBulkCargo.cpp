#include "LiquidBulkCargo.h"
#include <cmath>

namespace sphere::cargo {

LiquidBulkCargo::LiquidBulkCargo(CargoID variantId) {
    const LiquidVariantConfig* cfg = LiquidBulkVariants::FindVariant(variantId);
    if (cfg) {
        m_Def = LiquidBulkVariants::CreateCargoDef(*cfg);
    } else {
        m_Def.id = variantId;
        m_Def.name = GetCargoName(variantId);
        m_Def.category = "Liquid Bulk";
        m_Def.packingType = CargoPackingType::DRUM;
    }
}

std::shared_ptr<Mesh> LiquidBulkCargo::GenerateMesh(uint32_t lod) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    uint32_t radialSegments = (lod == 0) ? 36 : 24;
    uint32_t heightSegments = (lod == 0) ? 32 : 18;

    float radius = m_Def.dimensions.x * 0.5f;
    float halfH = m_Def.dimensions.z * 0.5f;

    // Procedural cylinder with rolling hoop ribs
    for (uint32_t iy = 0; iy <= heightSegments; ++iy) {
        float v = static_cast<float>(iy) / static_cast<float>(heightSegments);
        float y = -halfH + v * (2.0f * halfH);

        // Rolling hoops for 200L drums at 1/3 and 2/3 height
        float r = radius;
        if (m_Def.id == CargoID::OIL_DRUM_200L) {
            float hoop1 = std::exp(-std::pow((v - 0.35f) * 35.0f, 2.0f)) * 0.016f;
            float hoop2 = std::exp(-std::pow((v - 0.65f) * 35.0f, 2.0f)) * 0.016f;
            float chimeTop = std::exp(-std::pow((v - 0.98f) * 60.0f, 2.0f)) * 0.012f;
            float chimeBot = std::exp(-std::pow((v - 0.02f) * 60.0f, 2.0f)) * 0.012f;
            r += hoop1 + hoop2 + chimeTop + chimeBot;
        } else if (m_Def.id == CargoID::GAS_CYLINDER) {
            // Domed top and foot ring
            if (v > 0.85f) {
                float domeT = (v - 0.85f) / 0.15f;
                r *= std::sqrt(std::max(0.01f, 1.0f - domeT * domeT));
            }
        }

        for (uint32_t ix = 0; ix <= radialSegments; ++ix) {
            float u = static_cast<float>(ix) / static_cast<float>(radialSegments);
            float angle = u * 6.2831853f;

            float x = std::cos(angle) * r;
            float z = std::sin(angle) * r;

            Vertex vert;
            vert.position = Vec3(x, y, z);
            vert.uv[0] = u;
            vert.uv[1] = v;
            vert.color = m_Def.base_color;
            mesh->vertices.push_back(vert);
        }
    }

    // Indices for cylinder body
    for (uint32_t iy = 0; iy < heightSegments; ++iy) {
        for (uint32_t ix = 0; ix < radialSegments; ++ix) {
            uint32_t row0 = iy * (radialSegments + 1);
            uint32_t row1 = (iy + 1) * (radialSegments + 1);

            mesh->indices.push_back(row0 + ix);
            mesh->indices.push_back(row1 + ix);
            mesh->indices.push_back(row0 + ix + 1);

            mesh->indices.push_back(row0 + ix + 1);
            mesh->indices.push_back(row1 + ix);
            mesh->indices.push_back(row1 + ix + 1);
        }
    }

    // Top and Bottom end caps
    for (int cap = 0; cap < 2; ++cap) {
        float yCap = (cap == 0) ? halfH : -halfH;
        float yNorm = (cap == 0) ? 1.0f : -1.0f;
        uint32_t centerIdx = static_cast<uint32_t>(mesh->vertices.size());

        Vertex centerVert;
        centerVert.position = Vec3(0, yCap, 0);
        centerVert.normal = Vec3(0, yNorm, 0);
        centerVert.uv[0] = 0.5f;
        centerVert.uv[1] = 0.5f;
        centerVert.color = m_Def.base_color;
        mesh->vertices.push_back(centerVert);

        uint32_t ringStart = static_cast<uint32_t>(mesh->vertices.size());
        for (uint32_t ix = 0; ix <= radialSegments; ++ix) {
            float u = static_cast<float>(ix) / static_cast<float>(radialSegments);
            float angle = u * 6.2831853f;
            float x = std::cos(angle) * radius * 0.98f;
            float z = std::sin(angle) * radius * 0.98f;

            Vertex rimVert;
            rimVert.position = Vec3(x, yCap, z);
            rimVert.normal = Vec3(0, yNorm, 0);
            rimVert.uv[0] = 0.5f + std::cos(angle) * 0.45f;
            rimVert.uv[1] = 0.5f + std::sin(angle) * 0.45f;
            rimVert.color = m_Def.base_color;
            mesh->vertices.push_back(rimVert);
        }

        for (uint32_t ix = 0; ix < radialSegments; ++ix) {
            if (cap == 0) {
                mesh->indices.push_back(centerIdx);
                mesh->indices.push_back(ringStart + ix);
                mesh->indices.push_back(ringStart + ix + 1);
            } else {
                mesh->indices.push_back(centerIdx);
                mesh->indices.push_back(ringStart + ix + 1);
                mesh->indices.push_back(ringStart + ix);
            }
        }
    }

    mesh->CalculateSmoothNormals();
    mesh->CalculateTangents();
    mesh->CalculateBounds();

    m_CachedMesh = mesh;
    m_Bounds = mesh->bounds;
    return mesh;
}

bool LiquidBulkCargo::GenerateTextures(VkDevice device, VkQueue queue, VkCommandPool pool) {
    m_IsGenerated = true;
    return true;
}

} // namespace sphere::cargo
