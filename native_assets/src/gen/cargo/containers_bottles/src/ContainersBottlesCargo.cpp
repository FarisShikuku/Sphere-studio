#include "ContainersBottlesCargo.h"
#include <cmath>

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

    uint32_t radialSegs = (lod == 0) ? 32 : 20;
    uint32_t profileSegs = 24;

    float baseRadius = m_Def.dimensions.x * 0.5f;
    float totalH = m_Def.dimensions.z;

    // Lathe profile function r(v) along vertical axis (0.0 to 1.0)
    for (uint32_t ip = 0; ip <= profileSegs; ++ip) {
        float v = static_cast<float>(ip) / static_cast<float>(profileSegs);
        float y = v * totalH;

        float r = baseRadius;
        if (m_Def.id == CargoID::TIN_CAN) {
            // Straight cylinder with top & bottom rim chime
            if (v < 0.05f || v > 0.95f) r += 0.002f;
        } else if (m_Def.id == CargoID::JAR_GLASS) {
            // Wide squat jar with neck constriction at 85%
            if (v > 0.82f) r *= 0.85f;
        } else {
            // Bottle with heel pushup, body, shoulder (60%-80%), neck, lip
            if (v < 0.05f) {
                r = baseRadius * (0.88f + 0.12f * (v / 0.05f)); // heel fillet
            } else if (v < 0.60f) {
                r = baseRadius; // main label body
            } else if (v < 0.82f) {
                // shoulder curve transition to neck
                float st = (v - 0.60f) / 0.22f;
                r = baseRadius * (1.0f - st * 0.65f);
            } else if (v < 0.96f) {
                r = baseRadius * 0.35f; // neck cylinder
            } else {
                r = baseRadius * 0.40f; // crown cap lip finish
            }
        }

        for (uint32_t ir = 0; ir <= radialSegs; ++ir) {
            float u = static_cast<float>(ir) / static_cast<float>(radialSegs);
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

    // Indices for lathe surface
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
