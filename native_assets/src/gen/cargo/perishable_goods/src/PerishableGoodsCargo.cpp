#include "PerishableGoodsCargo.h"
#include <cmath>

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

static void AddProduceSphere(
    Mesh& mesh,
    const Vec3& center,
    float radius,
    const Vec4& color,
    uint32_t rings = 8,
    uint32_t sectors = 12
) {
    uint32_t base = static_cast<uint32_t>(mesh.vertices.size());

    for (uint32_t r = 0; r <= rings; ++r) {
        float phi = 3.14159265f * static_cast<float>(r) / static_cast<float>(rings);
        float y = std::cos(phi) * radius;
        float ringR = std::sin(phi) * radius;

        for (uint32_t s = 0; s <= sectors; ++s) {
            float theta = 6.2831853f * static_cast<float>(s) / static_cast<float>(sectors);
            float x = std::cos(theta) * ringR;
            float z = std::sin(theta) * ringR;

            Vec3 normal = Vec3(x, y, z).Normalized();
            Vertex vert;
            vert.position = center + Vec3(x, y, z);
            vert.normal = normal;
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

            mesh.indices.push_back(r0 + s);
            mesh.indices.push_back(r1 + s);
            mesh.indices.push_back(r0 + s + 1);

            mesh.indices.push_back(r0 + s + 1);
            mesh.indices.push_back(r1 + s);
            mesh.indices.push_back(r1 + s + 1);
        }
    }
}

std::shared_ptr<Mesh> PerishableGoodsCargo::GenerateMesh(uint32_t lod) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float halfL = m_Def.dimensions.x * 0.5f;
    float halfH = m_Def.dimensions.z * 0.5f;
    float halfW = m_Def.dimensions.y * 0.5f;

    // Outer crate box container base
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

    Vec4 containerColor = Vec4(0.25f, 0.28f, 0.32f, 1.0f); // Plastic crate frame
    for (int i = 0; i < 8; ++i) {
        Vertex v;
        v.position = corners[i];
        v.uv[0] = (i % 2 == 0) ? 0.0f : 1.0f;
        v.uv[1] = (i < 4) ? 0.0f : 1.0f;
        v.color = containerColor;
        mesh->vertices.push_back(v);
    }

    uint32_t cubeIndices[36] = {
        0, 2, 1, 0, 3, 2,
        4, 5, 6, 4, 6, 7,
        0, 1, 5, 0, 5, 4,
        3, 6, 2, 3, 7, 6,
        0, 4, 7, 0, 7, 3,
        1, 2, 6, 1, 6, 5
    };
    for (int i = 0; i < 36; ++i) {
        mesh->indices.push_back(base + cubeIndices[i]);
    }

    // Clustered produce spheres inside crate (e.g. fresh harvest tomatoes or potatoes)
    int nx = 4;
    int nz = 3;
    float sphereRadius = std::min(m_Def.dimensions.x / (nx * 2.1f), m_Def.dimensions.y / (nz * 2.1f));
    float startX = -halfL + sphereRadius * 1.1f;
    float startZ = -halfW + sphereRadius * 1.1f;
    float yPos = yMax * 0.85f;

    for (int ix = 0; ix < nx; ++ix) {
        for (int iz = 0; iz < nz; ++iz) {
            Vec3 pos(
                startX + ix * (sphereRadius * 2.1f),
                yPos,
                startZ + iz * (sphereRadius * 2.1f)
            );
            AddProduceSphere(*mesh, pos, sphereRadius, m_Def.base_color, 6, 8);
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
