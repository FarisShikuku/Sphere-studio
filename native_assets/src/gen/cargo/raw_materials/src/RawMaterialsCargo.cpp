#include "RawMaterialsCargo.h"
#include <cmath>

namespace sphere::cargo {

RawMaterialsCargo::RawMaterialsCargo(CargoID variantId) {
    const RawVariantConfig* cfg = RawMaterialsVariants::FindVariant(variantId);
    if (cfg) {
        m_Def = RawMaterialsVariants::CreateCargoDef(*cfg);
    } else {
        m_Def.id = variantId;
        m_Def.name = GetCargoName(variantId);
        m_Def.category = "Raw Materials";
        m_Def.packingType = CargoPackingType::RAW_STACK;
    }
}

std::shared_ptr<Mesh> RawMaterialsCargo::GenerateMesh(uint32_t lod) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    if (m_Def.id == CargoID::COAL_PILE || m_Def.id == CargoID::GRAVEL_PILE) {
        // Organic mound: conical Gaussian hill with vertex noise
        uint32_t rings = 20;
        uint32_t sectors = 28;
        float baseRadius = m_Def.dimensions.x * 0.5f;
        float peakHeight = m_Def.dimensions.z;

        for (uint32_t r = 0; r <= rings; ++r) {
            float fracR = static_cast<float>(r) / static_cast<float>(rings);
            float currentRadius = fracR * baseRadius;
            // Bell curve height
            float h = peakHeight * std::exp(-std::pow(fracR * 2.1f, 2.0f));

            for (uint32_t s = 0; s <= sectors; ++s) {
                float fracS = static_cast<float>(s) / static_cast<float>(sectors);
                float angle = fracS * 6.2831853f;

                // Noise perturb
                float noise = std::sin(angle * 7.0f + fracR * 12.0f) * 0.035f;
                float x = std::cos(angle) * (currentRadius + noise);
                float z = std::sin(angle) * (currentRadius + noise);
                float y = h + noise;

                Vertex vert;
                vert.position = Vec3(x, y, z);
                vert.uv[0] = 0.5f + (x / (baseRadius * 2.2f));
                vert.uv[1] = 0.5f + (z / (baseRadius * 2.2f));
                vert.color = m_Def.base_color;
                mesh->vertices.push_back(vert);
            }
        }

        for (uint32_t r = 0; r < rings; ++r) {
            for (uint32_t s = 0; s < sectors; ++s) {
                uint32_t r0 = r * (sectors + 1);
                uint32_t r1 = (r + 1) * (sectors + 1);

                mesh->indices.push_back(r0 + s);
                mesh->indices.push_back(r1 + s);
                mesh->indices.push_back(r0 + s + 1);

                mesh->indices.push_back(r0 + s + 1);
                mesh->indices.push_back(r1 + s);
                mesh->indices.push_back(r1 + s + 1);
            }
        }
    } else {
        // Horizontal Timber Log or Steel Rebar (along X axis)
        uint32_t lenSegs = 24;
        uint32_t radSegs = 20;
        float halfL = m_Def.dimensions.x * 0.5f;
        float radius = m_Def.dimensions.y * 0.5f;

        for (uint32_t il = 0; il <= lenSegs; ++il) {
            float u = static_cast<float>(il) / static_cast<float>(lenSegs);
            float x = -halfL + u * (2.0f * halfL);

            for (uint32_t ir = 0; ir <= radSegs; ++ir) {
                float v = static_cast<float>(ir) / static_cast<float>(radSegs);
                float angle = v * 6.2831853f;

                float barkRough = (m_Def.id == CargoID::WOOD_LOG_OAK) ?
                    (std::sin(angle * 12.0f) * 0.012f + std::sin(u * 30.0f) * 0.008f) : 0.0f;
                float r = radius + barkRough;

                float y = std::cos(angle) * r;
                float z = std::sin(angle) * r;

                Vertex vert;
                vert.position = Vec3(x, y + radius, z);
                vert.uv[0] = u;
                vert.uv[1] = v;
                vert.color = m_Def.base_color;
                mesh->vertices.push_back(vert);
            }
        }

        for (uint32_t il = 0; il < lenSegs; ++il) {
            for (uint32_t ir = 0; ir < radSegs; ++ir) {
                uint32_t row0 = il * (radSegs + 1);
                uint32_t row1 = (il + 1) * (radSegs + 1);

                mesh->indices.push_back(row0 + ir);
                mesh->indices.push_back(row1 + ir);
                mesh->indices.push_back(row0 + ir + 1);

                mesh->indices.push_back(row0 + ir + 1);
                mesh->indices.push_back(row1 + ir);
                mesh->indices.push_back(row1 + ir + 1);
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

bool RawMaterialsCargo::GenerateTextures(VkDevice device, VkQueue queue, VkCommandPool pool) {
    m_IsGenerated = true;
    return true;
}

} // namespace sphere::cargo
