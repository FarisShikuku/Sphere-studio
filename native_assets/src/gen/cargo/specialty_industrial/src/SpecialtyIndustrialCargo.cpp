#include "SpecialtyIndustrialCargo.h"
#include <cmath>

namespace sphere::cargo {

SpecialtyIndustrialCargo::SpecialtyIndustrialCargo(CargoID variantId) {
    const SpecialtyVariantConfig* cfg = SpecialtyIndustrialVariants::FindVariant(variantId);
    if (cfg) {
        m_Def = SpecialtyIndustrialVariants::CreateCargoDef(*cfg);
    } else {
        m_Def.id = variantId;
        m_Def.name = GetCargoName(variantId);
        m_Def.category = "Specialty Industrial";
        m_Def.packingType = CargoPackingType::PALLETIZED;
    }
}

std::shared_ptr<Mesh> SpecialtyIndustrialCargo::GenerateMesh(uint32_t lod) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    if (m_Def.id == CargoID::TYRE_STACK) {
        // Stack of 4 commercial truck tyres
        int numTyres = 4;
        float outerR = m_Def.dimensions.x * 0.5f;
        float innerR = outerR * 0.52f;
        float tyreH = m_Def.dimensions.z / static_cast<float>(numTyres);
        uint32_t radSegs = 28;

        for (int t = 0; t < numTyres; ++t) {
            float yBase = t * tyreH;
            for (int h = 0; h <= 2; ++h) {
                float y = yBase + (h * 0.5f) * tyreH;
                float r = (h == 1) ? outerR : (outerR * 0.94f); // crowned tread

                for (uint32_t i = 0; i <= radSegs; ++i) {
                    float u = static_cast<float>(i) / static_cast<float>(radSegs);
                    float angle = u * 6.2831853f;
                    float x = std::cos(angle) * r;
                    float z = std::sin(angle) * r;

                    Vertex vert;
                    vert.position = Vec3(x, y, z);
                    vert.uv[0] = u;
                    vert.uv[1] = (t * 0.25f) + (h * 0.125f);
                    vert.color = m_Def.base_color;
                    mesh->vertices.push_back(vert);
                }
            }

            uint32_t baseVert = t * 3 * (radSegs + 1);
            for (int h = 0; h < 2; ++h) {
                for (uint32_t i = 0; i < radSegs; ++i) {
                    uint32_t r0 = baseVert + h * (radSegs + 1);
                    uint32_t r1 = baseVert + (h + 1) * (radSegs + 1);

                    mesh->indices.push_back(r0 + i);
                    mesh->indices.push_back(r1 + i);
                    mesh->indices.push_back(r0 + i + 1);

                    mesh->indices.push_back(r0 + i + 1);
                    mesh->indices.push_back(r1 + i);
                    mesh->indices.push_back(r1 + i + 1);
                }
            }
        }
    } else if (m_Def.id == CargoID::STEEL_PIPE_6M) {
        // Long cylindrical pipe along X
        uint32_t lenSegs = 32;
        uint32_t radSegs = 24;
        float halfL = m_Def.dimensions.x * 0.5f;
        float r = m_Def.dimensions.y * 0.5f;

        for (uint32_t il = 0; il <= lenSegs; ++il) {
            float u = static_cast<float>(il) / static_cast<float>(lenSegs);
            float x = -halfL + u * (2.0f * halfL);

            for (uint32_t ir = 0; ir <= radSegs; ++ir) {
                float v = static_cast<float>(ir) / static_cast<float>(radSegs);
                float angle = v * 6.2831853f;
                float y = std::cos(angle) * r;
                float z = std::sin(angle) * r;

                Vertex vert;
                vert.position = Vec3(x, y + r, z);
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
    } else {
        // Masonry Brick Pallet cube block
        float halfL = m_Def.dimensions.x * 0.5f;
        float halfH = m_Def.dimensions.z * 0.5f;
        float halfW = m_Def.dimensions.y * 0.5f;

        // Base rectangular block
        float xMin = -halfL, xMax = halfL;
        float yMin = 0.0f, yMax = m_Def.dimensions.z;
        float zMin = -halfW, zMax = halfW;

        uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
        Vec3 corners[8] = {
            Vec3(xMin, yMin, zMin), Vec3(xMax, yMin, zMin),
            Vec3(xMax, yMax, zMin), Vec3(xMin, yMax, zMin),
            Vec3(xMin, yMin, zMax), Vec3(xMax, yMin, zMax),
            Vec3(xMax, yMax, zMax), Vec3(xMin, yMax, zMax)
        };

        for (int i = 0; i < 8; ++i) {
            Vertex v;
            v.position = corners[i];
            v.uv[0] = (i % 2 == 0) ? 0.0f : 1.0f;
            v.uv[1] = (i < 4) ? 0.0f : 1.0f;
            v.color = m_Def.base_color;
            mesh->vertices.push_back(v);
        }

        uint32_t cubeIndices[36] = {
            0, 2, 1, 0, 3, 2, // Front
            4, 5, 6, 4, 6, 7, // Back
            0, 1, 5, 0, 5, 4, // Bottom
            3, 6, 2, 3, 7, 6, // Top
            0, 4, 7, 0, 7, 3, // Left
            1, 2, 6, 1, 6, 5  // Right
        };
        for (int i = 0; i < 36; ++i) {
            mesh->indices.push_back(base + cubeIndices[i]);
        }
    }

    mesh->CalculateSmoothNormals();
    mesh->CalculateTangents();
    mesh->CalculateBounds();

    m_CachedMesh = mesh;
    m_Bounds = mesh->bounds;
    return mesh;
}

bool SpecialtyIndustrialCargo::GenerateTextures(VkDevice device, VkQueue queue, VkCommandPool pool) {
    m_IsGenerated = true;
    return true;
}

} // namespace sphere::cargo
