#include "PerishableGoodsCargo.h"
#include <cmath>
#include <algorithm>

namespace sphere::cargo {

PerishableGoodsCargo::PerishableGoodsCargo(CargoID variantId) {
    const PerishableVariantConfig* cfg = PerishableGoodsVariants::FindVariant(variantId);
    if (cfg) {
        m_Def = PerishableGoodsVariants::CreateCargoDef(*cfg);
    } else {
        m_Def.id = variantId;
        m_Def.name = GetCargoName(variantId);
        m_Def.category = "Perishable Goods";
        m_Def.packingType = CargoPackingType::PERISHABLE;
    }
}

static void AddLobedProduceSphere(
    Mesh& mesh, const Vec3& center, float radius,
    const Vec4& color, float lobeAmp, int lobes,
    uint32_t rings = 8, uint32_t sectors = 12
) {
    uint32_t base = static_cast<uint32_t>(mesh.vertices.size());

    for (uint32_t r = 0; r <= rings; ++r) {
        float phi = 3.14159265f * static_cast<float>(r) / static_cast<float>(rings);
        float sp = std::sin(phi);
        float cp = std::cos(phi);

        for (uint32_t s = 0; s <= sectors; ++s) {
            float theta = 6.2831853f * static_cast<float>(s) / static_cast<float>(sectors);
            float radMod = radius * (1.0f + lobeAmp * sp * sp * std::cos(lobes * theta));
            float x = std::cos(theta) * sp * radMod;
            float y = cp * radMod * 0.85f;
            float z = std::sin(theta) * sp * radMod;

            Vertex vert;
            vert.position = center + Vec3(x, y, z);
            vert.normal = Vec3(x, y, z).Normalized();
            vert.uv[0] = static_cast<float>(s) / static_cast<float>(sectors);
            vert.uv[1] = static_cast<float>(r) / static_cast<float>(rings);
            vert.color = color;
            mesh.vertices.push_back(vert);
        }
    }

    for (uint32_t r = 0; r < rings; ++r) {
        for (uint32_t s = 0; s < sectors; ++s) {
            uint32_t r0 = base + r * (sectors + 1);
            uint32_t r1 = base + (r + 1) * (sectors + 1);
            mesh.indices.push_back(r0 + s); mesh.indices.push_back(r0 + s + 1); mesh.indices.push_back(r1 + s);
            mesh.indices.push_back(r0 + s + 1); mesh.indices.push_back(r1 + s + 1); mesh.indices.push_back(r1 + s);
        }
    }
}

std::shared_ptr<Mesh> PerishableGoodsCargo::GenerateMesh(uint32_t lod) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float halfL = m_Def.dimensions.x * 0.5f;
    float halfW = m_Def.dimensions.y * 0.5f;
    float H = m_Def.dimensions.z;

    if (m_Def.id == CargoID::PRODUCE_BAG_POTATO) {
        // Superellipsoid ventilated mesh bag with clustered tuber lumps
        uint32_t segX = 24, segZ = 18;
        for (int side = 0; side < 2; ++side) {
            float ySign = (side == 0) ? 1.0f : -1.0f;
            uint32_t baseIdx = static_cast<uint32_t>(mesh->vertices.size());
            for (uint32_t iz = 0; iz <= segZ; ++iz) {
                float v = static_cast<float>(iz) / static_cast<float>(segZ);
                float eta = 2.0f * v - 1.0f;
                for (uint32_t ix = 0; ix <= segX; ++ix) {
                    float u = static_cast<float>(ix) / static_cast<float>(segX);
                    float xi = 2.0f * u - 1.0f;
                    float env = std::pow(std::max(0.0f, (1.0f - std::pow(std::abs(xi), 2.4f)) * (1.0f - std::pow(std::abs(eta), 2.2f))), 0.42f);
                    float tuberLump = 0.018f * env * std::abs(std::sin(xi * 9.0f) * std::cos(eta * 11.0f));
                    Vertex vert;
                    vert.position = Vec3(xi * halfL, ySign * (env * H * 0.5f + tuberLump) + H * 0.5f, eta * halfW);
                    vert.uv[0] = u; vert.uv[1] = v;
                    vert.color = m_Def.base_color;
                    mesh->vertices.push_back(vert);
                }
            }
            for (uint32_t iz = 0; iz < segZ; ++iz) {
                for (uint32_t ix = 0; ix < segX; ++ix) {
                    uint32_t r0 = baseIdx + iz * (segX + 1), r1 = baseIdx + (iz + 1) * (segX + 1);
                    if (side == 0) {
                        mesh->indices.push_back(r0 + ix); mesh->indices.push_back(r1 + ix); mesh->indices.push_back(r0 + ix + 1);
                        mesh->indices.push_back(r0 + ix + 1); mesh->indices.push_back(r1 + ix); mesh->indices.push_back(r1 + ix + 1);
                    } else {
                        mesh->indices.push_back(r0 + ix); mesh->indices.push_back(r0 + ix + 1); mesh->indices.push_back(r1 + ix);
                        mesh->indices.push_back(r0 + ix + 1); mesh->indices.push_back(r1 + ix + 1); mesh->indices.push_back(r1 + ix);
                    }
                }
            }
        }
    } else {
        // Crate/Carton shell + clustered lobed produce
        uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
        Vec3 corners[8] = {
            Vec3(-halfL, 0.0f, -halfW), Vec3(halfL, 0.0f, -halfW),
            Vec3(halfL, H * 0.85f, -halfW), Vec3(-halfL, H * 0.85f, -halfW),
            Vec3(-halfL, 0.0f, halfW), Vec3(halfL, 0.0f, halfW),
            Vec3(halfL, H * 0.85f, halfW), Vec3(-halfL, H * 0.85f, halfW)
        };
        Vec4 containerColor = (m_Def.id == CargoID::MEAT_BOX) ? m_Def.base_color : Vec4(0.18f, 0.22f, 0.28f, 1.0f);
        for (int i = 0; i < 8; ++i) {
            Vertex v;
            v.position = corners[i];
            v.uv[0] = (i % 2 == 0) ? 0.0f : 1.0f;
            v.uv[1] = (i < 4) ? 0.0f : 1.0f;
            v.color = containerColor;
            mesh->vertices.push_back(v);
        }
        uint32_t cubeIndices[36] = {
            0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7, 0, 1, 5, 0, 5, 4,
            3, 6, 2, 3, 7, 6, 0, 4, 7, 0, 7, 3, 1, 2, 6, 1, 6, 5
        };
        for (int i = 0; i < 36; ++i) mesh->indices.push_back(base + cubeIndices[i]);

        if (m_Def.id == CargoID::PRODUCE_CRATE_TOMATO || m_Def.id == CargoID::FISH_CRATE) {
            int nx = 4, nz = 3;
            float sphereRadius = std::min(m_Def.dimensions.x / (nx * 2.2f), m_Def.dimensions.y / (nz * 2.2f));
            float startX = -halfL + sphereRadius * 1.15f;
            float startZ = -halfW + sphereRadius * 1.15f;
            for (int ix = 0; ix < nx; ++ix) {
                for (int iz = 0; iz < nz; ++iz) {
                    Vec3 pos(startX + ix * (sphereRadius * 2.1f), H * 0.85f, startZ + iz * (sphereRadius * 2.1f));
                    AddLobedProduceSphere(*mesh, pos, sphereRadius, m_Def.base_color, 0.08f, 5, 6, 10);
                }
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

bool PerishableGoodsCargo::GenerateTextures(VkDevice device, VkQueue queue, VkCommandPool pool) {
    m_IsGenerated = true;
    return true;
}

} // namespace sphere::cargo
