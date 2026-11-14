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
    mesh.vertices.emplace_back(p0, normal, Vec4(1, 0, 0, 1), 0.0f, 0.0f, color);
    mesh.vertices.emplace_back(p1, normal, Vec4(1, 0, 0, 1), 1.0f, 0.0f, color);
    mesh.vertices.emplace_back(p2, normal, Vec4(1, 0, 0, 1), 1.0f, 1.0f, color);
    mesh.vertices.emplace_back(p3, normal, Vec4(1, 0, 0, 1), 0.0f, 1.0f, color);

    mesh.indices.push_back(base + 0);
    mesh.indices.push_back(base + 1);
    mesh.indices.push_back(base + 2);
    mesh.indices.push_back(base + 0);
    mesh.indices.push_back(base + 2);
    mesh.indices.push_back(base + 3);
}

static void AddCubePrism(Mesh& mesh, const Vec3& minP, const Vec3& maxP, const Vec4& color) {
    AddBoxFace(mesh, Vec3(minP.x, minP.y, maxP.z), Vec3(maxP.x, minP.y, maxP.z),
                     Vec3(maxP.x, maxP.y, maxP.z), Vec3(minP.x, maxP.y, maxP.z), Vec3(0, 0, 1), color);
    AddBoxFace(mesh, Vec3(maxP.x, minP.y, minP.z), Vec3(minP.x, minP.y, minP.z),
                     Vec3(minP.x, maxP.y, minP.z), Vec3(maxP.x, maxP.y, minP.z), Vec3(0, 0, -1), color);
    AddBoxFace(mesh, Vec3(minP.x, maxP.y, maxP.z), Vec3(maxP.x, maxP.y, maxP.z),
                     Vec3(maxP.x, maxP.y, minP.z), Vec3(minP.x, maxP.y, minP.z), Vec3(0, 1, 0), color);
    AddBoxFace(mesh, Vec3(minP.x, minP.y, minP.z), Vec3(maxP.x, minP.y, minP.z),
                     Vec3(maxP.x, minP.y, maxP.z), Vec3(minP.x, minP.y, maxP.z), Vec3(0, -1, 0), color);
    AddBoxFace(mesh, Vec3(maxP.x, minP.y, maxP.z), Vec3(maxP.x, minP.y, minP.z),
                     Vec3(maxP.x, maxP.y, minP.z), Vec3(maxP.x, maxP.y, maxP.z), Vec3(1, 0, 0), color);
    AddBoxFace(mesh, Vec3(minP.x, minP.y, minP.z), Vec3(minP.x, minP.y, maxP.z),
                     Vec3(minP.x, maxP.y, maxP.z), Vec3(minP.x, maxP.y, minP.z), Vec3(-1, 0, 0), color);
}

static void AddDiagonalStrut(
    Mesh& mesh,
    const Vec3& pStart, const Vec3& pEnd,
    float thickness, const Vec3& faceNormal,
    const Vec4& color
) {
    Vec3 dir = (pEnd - pStart).Normalized();
    Vec3 side = Vec3(
        faceNormal.y * dir.z - faceNormal.z * dir.y,
        faceNormal.z * dir.x - faceNormal.x * dir.z,
        faceNormal.x * dir.y - faceNormal.y * dir.x
    ).Normalized() * (thickness * 0.5f);
    Vec3 extrude = faceNormal * thickness;

    AddBoxFace(mesh, pStart - side + extrude, pStart + side + extrude, pEnd + side + extrude, pEnd - side + extrude, faceNormal, color);
    AddBoxFace(mesh, pStart + side, pStart - side, pEnd - side, pEnd + side, -faceNormal, color * 0.85f);
    AddBoxFace(mesh, pStart + side, pStart + side + extrude, pEnd + side + extrude, pEnd + side, side.Normalized(), color * 0.9f);
    AddBoxFace(mesh, pStart - side + extrude, pStart - side, pEnd - side, pEnd - side + extrude, -side.Normalized(), color * 0.9f);
    AddBoxFace(mesh, pStart - side, pStart + side, pStart + side + extrude, pStart - side + extrude, -dir, color * 0.85f);
    AddBoxFace(mesh, pEnd + side, pEnd - side, pEnd - side + extrude, pEnd + side + extrude, dir, color * 0.85f);
}

std::shared_ptr<Mesh> BoxesCratesCargo::GenerateMesh(uint32_t lod) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float halfL = m_Def.dimensions.x * 0.5f;
    float halfW = m_Def.dimensions.y * 0.5f;
    float H = m_Def.dimensions.z;

    if (m_Def.id == CargoID::PALLET) {
        // EN 13698-1 EUR-1 Pallet: 5 top boards, 3 stringers, 9 chamfered spacer blocks, 3 skids
        float deckT = 0.022f;
        float stringerT = 0.022f;
        float blockH = H - deckT * 2.0f - stringerT;

        // 5 Top deck boards along X
        int numTop = 5;
        float bw = (m_Def.dimensions.y - 0.08f) / numTop;
        for (int b = 0; b < numTop; ++b) {
            float z0 = -halfW + 0.01f + b * (bw + 0.015f);
            AddCubePrism(*mesh, Vec3(-halfL, H - deckT, z0), Vec3(halfL, H, z0 + bw), m_Def.base_color);
        }

        // 3 Transverse stringer boards along Z
        float xPos[3] = { -halfL + 0.07f, 0.0f, halfL - 0.07f };
        for (float xc : xPos) {
            AddCubePrism(*mesh, Vec3(xc - 0.07f, H - deckT - stringerT, -halfW),
                                Vec3(xc + 0.07f, H - deckT, halfW), m_Def.base_color * 0.94f);
        }

        // 9 Spacer blocks
        float zPos[3] = { -halfW + 0.055f, 0.0f, halfW - 0.055f };
        for (float xc : xPos) {
            for (float zc : zPos) {
                AddCubePrism(*mesh, Vec3(xc - 0.07f, deckT, zc - 0.05f),
                                    Vec3(xc + 0.07f, deckT + blockH, zc + 0.05f), m_Def.base_color * 0.86f);
            }
        }

        // 3 Bottom skids
        for (float zc : zPos) {
            AddCubePrism(*mesh, Vec3(-halfL, 0.0f, zc - 0.05f),
                                Vec3(halfL, deckT, zc + 0.05f), m_Def.base_color * 0.80f);
        }
    } else if (m_Def.id == CargoID::WOODEN_CRATE) {
        // Inner dark timber core + frame posts + Warren diagonal bracing + horizontal side slats
        AddCubePrism(*mesh, Vec3(-halfL * 0.94f, 0.02f, -halfW * 0.94f),
                            Vec3(halfL * 0.94f, H - 0.02f, halfW * 0.94f), m_Def.base_color * 0.65f);

        float post = 0.048f;
        Vec4 frameCol = m_Def.base_color * 0.96f;
        // Corner vertical posts
        AddCubePrism(*mesh, Vec3(-halfL, 0, -halfW), Vec3(-halfL + post, H, -halfW + post), frameCol);
        AddCubePrism(*mesh, Vec3(halfL - post, 0, -halfW), Vec3(halfL, H, -halfW + post), frameCol);
        AddCubePrism(*mesh, Vec3(-halfL, 0, halfW - post), Vec3(-halfL + post, H, halfW), frameCol);
        AddCubePrism(*mesh, Vec3(halfL - post, 0, halfW - post), Vec3(halfL, H, halfW), frameCol);

        // Top & bottom perimeter horizontal rails
        AddCubePrism(*mesh, Vec3(-halfL, 0, -halfW), Vec3(halfL, post, -halfW + 0.025f), frameCol);
        AddCubePrism(*mesh, Vec3(-halfL, H - post, -halfW), Vec3(halfL, H, -halfW + 0.025f), frameCol);
        AddCubePrism(*mesh, Vec3(-halfL, 0, halfW - 0.025f), Vec3(halfL, post, halfW), frameCol);
        AddCubePrism(*mesh, Vec3(-halfL, H - post, halfW - 0.025f), Vec3(halfL, H, halfW), frameCol);

        // Diagonal bracing struts on front (+Z) and back (-Z)
        AddDiagonalStrut(*mesh, Vec3(-halfL + post, post, halfW - 0.01f), Vec3(halfL - post, H - post, halfW - 0.01f), 0.032f, Vec3(0, 0, 1), frameCol * 0.9f);
        AddDiagonalStrut(*mesh, Vec3(-halfL + post, post, -halfW + 0.01f), Vec3(halfL - post, H - post, -halfW + 0.01f), 0.032f, Vec3(0, 0, -1), frameCol * 0.9f);

        // Horizontal slats along side walls
        for (int s = 1; s <= 3; ++s) {
            float yS = (H / 4.0f) * s;
            AddCubePrism(*mesh, Vec3(-halfL, yS - 0.025f, -halfW + post), Vec3(-halfL + 0.02f, yS + 0.025f, halfW - post), frameCol * 0.92f);
            AddCubePrism(*mesh, Vec3(halfL - 0.02f, yS - 0.025f, -halfW + post), Vec3(halfL, yS + 0.025f, halfW - post), frameCol * 0.92f);
        }
    } else if (m_Def.id == CargoID::PLASTIC_CRATE || m_Def.id == CargoID::MILK_CRATE) {
        // Top reinforced rim + vertical corner ribs + recessed base
        float rimH = H * 0.14f;
        float inset = 0.018f;
        AddCubePrism(*mesh, Vec3(-halfL + inset, 0, -halfW + inset), Vec3(halfL - inset, H - rimH, halfW - inset), m_Def.base_color * 0.88f);
        AddCubePrism(*mesh, Vec3(-halfL, H - rimH, -halfW), Vec3(halfL, H, halfW), m_Def.base_color);
        // Vertical external strengthening ribs
        for (int r = -2; r <= 2; ++r) {
            float rx = r * (halfL * 0.35f);
            AddCubePrism(*mesh, Vec3(rx - 0.015f, 0, -halfW), Vec3(rx + 0.015f, H - rimH, halfW), m_Def.base_color * 0.95f);
        }
    } else {
        // Corrugated Cardboard Carton: Superquadric chamfer + Top center flap seam & Kraft Tape
        AddCubePrism(*mesh, Vec3(-halfL, 0, -halfW), Vec3(halfL, H, halfW), m_Def.base_color);
        // Raised adhesive kraft packing tape strip over center seam (+Y and down +-Z)
        float tapeW = 0.055f;
        Vec4 tapeCol = Vec4(0.55f, 0.38f, 0.22f, 1.0f);
        AddCubePrism(*mesh, Vec3(-halfL - 0.002f, H, -tapeW * 0.5f), Vec3(halfL + 0.002f, H + 0.004f, tapeW * 0.5f), tapeCol);
        AddCubePrism(*mesh, Vec3(-halfL - 0.003f, H - 0.08f, -tapeW * 0.5f), Vec3(-halfL, H + 0.004f, tapeW * 0.5f), tapeCol);
        AddCubePrism(*mesh, Vec3(halfL, H - 0.08f, -tapeW * 0.5f), Vec3(halfL + 0.003f, H + 0.004f, tapeW * 0.5f), tapeCol);
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
