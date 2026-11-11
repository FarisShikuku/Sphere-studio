#include "BoxesCratesCargo.h"
#include <cmath>

namespace sphere::cargo {

BoxesCratesCargo::BoxesCratesCargo(CargoID variantId) {
    const BoxVariantConfig* cfg = BoxesCratesVariants::FindVariant(variantId);
    if (cfg) {
        m_Def = BoxesCratesVariants::CreateCargoDef(*cfg);
    } else {
        m_Def.id = variantId;
        m_Def.name = GetCargoName(variantId);
        m_Def.category = "Boxes & Crates";
        m_Def.packingType = (variantId == CargoID::PALLET) ? CargoPackingType::PALLETIZED : CargoPackingType::CRATE;
    }
}

static void AddBoxFace(
    Mesh& mesh,
    const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3,
    const Vec3& normal,
    const Vec4& color
) {
    uint32_t base = static_cast<uint32_t>(mesh.vertices.size());

    Vertex v0(p0, normal, Vec4(1, 0, 0, 1), 0.0f, 0.0f, color);
    Vertex v1(p1, normal, Vec4(1, 0, 0, 1), 1.0f, 0.0f, color);
    Vertex v2(p2, normal, Vec4(1, 0, 0, 1), 1.0f, 1.0f, color);
    Vertex v3(p3, normal, Vec4(1, 0, 0, 1), 0.0f, 1.0f, color);

    mesh.vertices.push_back(v0);
    mesh.vertices.push_back(v1);
    mesh.vertices.push_back(v2);
    mesh.vertices.push_back(v3);

    mesh.indices.push_back(base + 0);
    mesh.indices.push_back(base + 1);
    mesh.indices.push_back(base + 2);

    mesh.indices.push_back(base + 0);
    mesh.indices.push_back(base + 2);
    mesh.indices.push_back(base + 3);
}

static void AddCubePrism(
    Mesh& mesh,
    const Vec3& minP, const Vec3& maxP,
    const Vec4& color
) {
    // Front (+Z)
    AddBoxFace(mesh, Vec3(minP.x, minP.y, maxP.z), Vec3(maxP.x, minP.y, maxP.z),
                     Vec3(maxP.x, maxP.y, maxP.z), Vec3(minP.x, maxP.y, maxP.z),
                     Vec3(0, 0, 1), color);
    // Back (-Z)
    AddBoxFace(mesh, Vec3(maxP.x, minP.y, minP.z), Vec3(minP.x, minP.y, minP.z),
                     Vec3(minP.x, maxP.y, minP.z), Vec3(maxP.x, maxP.y, minP.z),
                     Vec3(0, 0, -1), color);
    // Top (+Y)
    AddBoxFace(mesh, Vec3(minP.x, maxP.y, maxP.z), Vec3(maxP.x, maxP.y, maxP.z),
                     Vec3(maxP.x, maxP.y, minP.z), Vec3(minP.x, maxP.y, minP.z),
                     Vec3(0, 1, 0), color);
    // Bottom (-Y)
    AddBoxFace(mesh, Vec3(minP.x, minP.y, minP.z), Vec3(maxP.x, minP.y, minP.z),
                     Vec3(maxP.x, minP.y, maxP.z), Vec3(minP.x, minP.y, maxP.z),
                     Vec3(0, -1, 0), color);
    // Right (+X)
    AddBoxFace(mesh, Vec3(maxP.x, minP.y, maxP.z), Vec3(maxP.x, minP.y, minP.z),
                     Vec3(maxP.x, maxP.y, minP.z), Vec3(maxP.x, maxP.y, maxP.z),
                     Vec3(1, 0, 0), color);
    // Left (-X)
    AddBoxFace(mesh, Vec3(minP.x, minP.y, minP.z), Vec3(minP.x, minP.y, maxP.z),
                     Vec3(minP.x, maxP.y, maxP.z), Vec3(minP.x, maxP.y, minP.z),
                     Vec3(-1, 0, 0), color);
}

std::shared_ptr<Mesh> BoxesCratesCargo::GenerateMesh(uint32_t lod) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float halfL = m_Def.dimensions.x * 0.5f;
    float halfH = m_Def.dimensions.z * 0.5f; // vertical height in Y
    float halfW = m_Def.dimensions.y * 0.5f;

    if (m_Def.id == CargoID::PALLET) {
        // Detailed EUR Pallet: Top Deck boards, 3 stringer runners, bottom deck boards
        float deckThickness = 0.022f;
        float blockHeight = m_Def.dimensions.z - deckThickness * 2.0f;

        // Top 5 boards along X
        int numTopBoards = 5;
        float boardWidth = (m_Def.dimensions.y - 0.08f) / numTopBoards;
        for (int b = 0; b < numTopBoards; ++b) {
            float z0 = -halfW + 0.01f + b * (boardWidth + 0.015f);
            float z1 = z0 + boardWidth;
            AddCubePrism(*mesh,
                Vec3(-halfL, halfH - deckThickness, z0),
                Vec3(halfL, halfH, z1),
                m_Def.base_color);
        }

        // 3 Longitudinal runners / blocks
        float runnerW = 0.10f;
        float zPositions[3] = { -halfW + 0.05f, 0.0f, halfW - 0.05f };
        for (float zc : zPositions) {
            AddCubePrism(*mesh,
                Vec3(-halfL, -halfH + deckThickness, zc - runnerW * 0.5f),
                Vec3(halfL, halfH - deckThickness, zc + runnerW * 0.5f),
                m_Def.base_color * 0.9f);
        }

        // Bottom 3 skids
        for (float zc : zPositions) {
            AddCubePrism(*mesh,
                Vec3(-halfL, -halfH, zc - runnerW * 0.5f),
                Vec3(halfL, -halfH + deckThickness, zc + runnerW * 0.5f),
                m_Def.base_color * 0.85f);
        }
    } else if (m_Def.id == CargoID::WOODEN_CRATE) {
        // Solid body with exterior corner cleats and cross diagonals
        AddCubePrism(*mesh, Vec3(-halfL, -halfH, -halfW), Vec3(halfL, halfH, halfW), m_Def.base_color);
        // Corner wooden struts
        float strut = 0.04f;
        Vec4 strutCol = m_Def.base_color * 0.82f;
        AddCubePrism(*mesh, Vec3(-halfL - 0.005f, -halfH, -halfW - 0.005f), Vec3(-halfL + strut, halfH, -halfW + strut), strutCol);
        AddCubePrism(*mesh, Vec3(halfL - strut, -halfH, -halfW - 0.005f), Vec3(halfL + 0.005f, halfH, -halfW + strut), strutCol);
        AddCubePrism(*mesh, Vec3(-halfL - 0.005f, -halfH, halfW - strut), Vec3(-halfL + strut, halfH, halfW + 0.005f), strutCol);
        AddCubePrism(*mesh, Vec3(halfL - strut, -halfH, halfW - strut), Vec3(halfL + 0.005f, halfH, halfW + 0.005f), strutCol);
    } else {
        // Standard Cardboard Box or Molded Crate
        AddCubePrism(*mesh, Vec3(-halfL, -halfH, -halfW), Vec3(halfL, halfH, halfW), m_Def.base_color);
    }

    mesh->CalculateSmoothNormals();
    mesh->CalculateTangents();
    mesh->CalculateBounds();

    m_CachedMesh = mesh;
    m_Bounds = mesh->bounds;
    return mesh;
}

bool BoxesCratesCargo::GenerateTextures(VkDevice device, VkQueue queue, VkCommandPool pool) {
    m_IsGenerated = true;
    return true;
}

} // namespace sphere::cargo
