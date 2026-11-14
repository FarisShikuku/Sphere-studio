#include "BuildingMaterialsCargo.h"
#include <cmath>
#include <algorithm>

namespace sphere::cargo {

namespace {
void AppendBoxMesh(
    Mesh* mesh,
    const Vec3& minP, const Vec3& maxP,
    const Vec4& color
) {
    uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
    Vec3 corners[8] = {
        Vec3(minP.x, minP.y, maxP.z), Vec3(maxP.x, minP.y, maxP.z),
        Vec3(maxP.x, maxP.y, maxP.z), Vec3(minP.x, maxP.y, maxP.z),
        Vec3(minP.x, minP.y, minP.z), Vec3(maxP.x, minP.y, minP.z),
        Vec3(maxP.x, maxP.y, minP.z), Vec3(minP.x, maxP.y, minP.z)
    };
    for (int i = 0; i < 8; ++i) {
        Vertex v;
        v.position = corners[i];
        v.uv[0] = (i % 2 == 0) ? 0.0f : 1.0f;
        v.uv[1] = (i < 4) ? 0.0f : 1.0f;
        v.color = color;
        mesh->vertices.push_back(v);
    }
    const uint32_t tris[36] = {
        0, 1, 2, 0, 2, 3, // +Z
        5, 4, 7, 5, 7, 6, // -Z
        3, 2, 6, 3, 6, 7, // +Y
        4, 5, 1, 4, 1, 0, // -Y
        1, 5, 6, 1, 6, 2, // +X
        4, 0, 3, 4, 3, 7  // -X
    };
    for (int i = 0; i < 36; ++i) mesh->indices.push_back(base + tris[i]);
}
} // namespace

BuildingMaterialsCargo::BuildingMaterialsCargo(CargoID variantId) {
    const BuildingMaterialVariantConfig* cfg = BuildingMaterialsVariants::FindVariant(variantId);
    if (cfg) {
        m_Def = BuildingMaterialsVariants::CreateCargoDef(*cfg);
    } else {
        m_Def.id = variantId;
        m_Def.name = GetCargoName(variantId);
        m_Def.category = "Building Materials";
        m_Def.packingType = CargoPackingType::PALLETIZED;
    }
}

std::shared_ptr<Mesh> BuildingMaterialsCargo::GenerateMesh(uint32_t lod) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    float L = m_Def.dimensions.x;
    float W = m_Def.dimensions.y;
    float H = m_Def.dimensions.z;

    switch (m_Def.id) {
        case CargoID::STEEL_IBEAM_6M: {
            // Structural Wide-Flange HEA I-Beam (Top flange, bottom flange, central web + quartic fillet)
            float flangeT = H * 0.14f;
            float webT = W * 0.12f;
            AppendBoxMesh(mesh.get(), Vec3(-L * 0.5f, 0.0f, -W * 0.5f), Vec3(L * 0.5f, flangeT, W * 0.5f), m_Def.base_color);
            AppendBoxMesh(mesh.get(), Vec3(-L * 0.5f, H - flangeT, -W * 0.5f), Vec3(L * 0.5f, H, W * 0.5f), m_Def.base_color);
            AppendBoxMesh(mesh.get(), Vec3(-L * 0.5f, flangeT, -webT * 0.5f), Vec3(L * 0.5f, H - flangeT, webT * 0.5f), m_Def.base_color);
            break;
        }
        case CargoID::TIMBER_GLULAM_BEAM: {
            // 6 Laminated Timber Lamellas with parabolic camber delta(x) = d0*(1 - (2x/L)^2)
            uint32_t segsX = 16;
            float cam = H * 0.08f;
            for (int l = 0; l < 6; ++l) {
                float y0 = (static_cast<float>(l) / 6.0f) * H;
                float y1 = (static_cast<float>(l + 1) / 6.0f) * H;
                for (uint32_t ix = 0; ix < segsX; ++ix) {
                    float u0 = static_cast<float>(ix) / static_cast<float>(segsX);
                    float u1 = static_cast<float>(ix + 1) / static_cast<float>(segsX);
                    float x0 = -L * 0.5f + u0 * L;
                    float x1 = -L * 0.5f + u1 * L;
                    float dy = cam * (1.0f - (2.0f * (0.5f * (u0 + u1)) - 1.0f) * (2.0f * (0.5f * (u0 + u1)) - 1.0f));
                    Vec4 col = (l % 2 == 0) ? m_Def.base_color : m_Def.label_color;
                    AppendBoxMesh(mesh.get(), Vec3(x0, y0 + dy, -W * 0.5f), Vec3(x1, y1 + dy - 0.002f, W * 0.5f), col);
                }
            }
            break;
        }
        case CargoID::SILICA_SAND_HEAP: {
            // Angle-of-repose bell curve h(r,theta) = H*(1 - (r/R)^2)^1.15 + pour ripple
            uint32_t rings = 18, secs = 32;
            float baseR = L * 0.5f;
            for (uint32_t r = 0; r <= rings; ++r) {
                float fr = static_cast<float>(r) / static_cast<float>(rings);
                float cr = fr * baseR;
                float h = std::pow(std::max(0.0f, 1.0f - fr * fr), 1.15f) * H;
                for (uint32_t s = 0; s <= secs; ++s) {
                    float ang = (static_cast<float>(s) / static_cast<float>(secs)) * 6.2831853f;
                    float ripple = 0.015f * H * std::sin(22.0f * fr) * std::cos(5.0f * ang);
                    Vertex v;
                    v.position = Vec3(cr * std::cos(ang), h + ripple, cr * std::sin(ang));
                    v.uv[0] = fr;
                    v.uv[1] = static_cast<float>(s) / static_cast<float>(secs);
                    v.color = m_Def.base_color;
                    mesh->vertices.push_back(v);
                }
            }
            for (uint32_t r = 0; r < rings; ++r) {
                for (uint32_t s = 0; s < secs; ++s) {
                    uint32_t r0 = r * (secs + 1), r1 = (r + 1) * (secs + 1);
                    mesh->indices.push_back(r0 + s); mesh->indices.push_back(r0 + s + 1); mesh->indices.push_back(r1 + s);
                    mesh->indices.push_back(r0 + s + 1); mesh->indices.push_back(r1 + s + 1); mesh->indices.push_back(r1 + s);
                }
            }
            // Closed base disc (-Y)
            uint32_t outerRing = rings * (secs + 1);
            Vertex bc{}; bc.position = Vec3(0.0f, 0.0f, 0.0f); bc.uv[0] = 0.5f; bc.uv[1] = 0.5f; bc.color = m_Def.base_color * 0.78f;
            uint32_t bcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(bc);
            for (uint32_t s = 0; s < secs; ++s) {
                mesh->indices.push_back(bcIdx); mesh->indices.push_back(outerRing + s); mesh->indices.push_back(outerRing + s + 1);
            }
            break;
        }
        case CargoID::SILICA_SAND_FIBC_1T: {
            // Superquadric p=4 FIBC dumpy tote + closed bottom/top caps + 4 corner lift loops
            uint32_t hSegs = 12, rSegs = 24;
            for (uint32_t iy = 0; iy <= hSegs; ++iy) {
                float v = static_cast<float>(iy) / static_cast<float>(hSegs);
                float y = v * H * 0.85f;
                float bulge = 1.0f + 0.07f * std::sin(v * 3.14159f);
                for (uint32_t ix = 0; ix <= rSegs; ++ix) {
                    float th = (static_cast<float>(ix) / static_cast<float>(rSegs)) * 6.2831853f;
                    float ct = std::cos(th), st = std::sin(th);
                    float sr = std::pow(std::pow(std::abs(ct), 4.0f) + std::pow(std::abs(st), 4.0f), -0.25f);
                    Vertex vert;
                    vert.position = Vec3(ct * sr * (L * 0.48f) * bulge, y, st * sr * (W * 0.48f) * bulge);
                    vert.uv[0] = static_cast<float>(ix) / static_cast<float>(rSegs);
                    vert.uv[1] = v;
                    vert.color = m_Def.base_color;
                    mesh->vertices.push_back(vert);
                }
            }
            for (uint32_t iy = 0; iy < hSegs; ++iy) {
                for (uint32_t ix = 0; ix < rSegs; ++ix) {
                    uint32_t r0 = iy * (rSegs + 1), r1 = (iy + 1) * (rSegs + 1);
                    mesh->indices.push_back(r0 + ix); mesh->indices.push_back(r1 + ix); mesh->indices.push_back(r0 + ix + 1);
                    mesh->indices.push_back(r0 + ix + 1); mesh->indices.push_back(r1 + ix); mesh->indices.push_back(r1 + ix + 1);
                }
            }
            uint32_t botRing = 0;
            uint32_t topRing = hSegs * (rSegs + 1);
            Vertex bc{}; bc.position = Vec3(0.0f, 0.0f, 0.0f); bc.color = m_Def.base_color * 0.85f;
            Vertex tc{}; tc.position = Vec3(0.0f, H * 0.88f, 0.0f); tc.color = m_Def.base_color * 0.96f;
            uint32_t bcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(bc);
            uint32_t tcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(tc);
            for (uint32_t ix = 0; ix < rSegs; ++ix) {
                mesh->indices.push_back(bcIdx); mesh->indices.push_back(botRing + ix); mesh->indices.push_back(botRing + ix + 1);
                mesh->indices.push_back(tcIdx); mesh->indices.push_back(topRing + ix + 1); mesh->indices.push_back(topRing + ix);
            }
            // 4 Blue corner lift straps
            float lx = L * 0.36f, lz = W * 0.36f;
            AppendBoxMesh(mesh.get(), Vec3(-lx - 0.04f, H * 0.82f, -lz - 0.04f), Vec3(-lx + 0.04f, H, -lz + 0.04f), m_Def.label_color);
            AppendBoxMesh(mesh.get(), Vec3(lx - 0.04f, H * 0.82f, -lz - 0.04f), Vec3(lx + 0.04f, H, -lz + 0.04f), m_Def.label_color);
            AppendBoxMesh(mesh.get(), Vec3(-lx - 0.04f, H * 0.82f, lz - 0.04f), Vec3(-lx + 0.04f, H, lz + 0.04f), m_Def.label_color);
            AppendBoxMesh(mesh.get(), Vec3(lx - 0.04f, H * 0.82f, lz - 0.04f), Vec3(lx + 0.04f, H, lz + 0.04f), m_Def.label_color);
            break;
        }
        case CargoID::FLOAT_GLASS_STILLAGE: {
            // Base stillage + upright A-frame stanchions + 5 solid emerald architectural glass sheets
            AppendBoxMesh(mesh.get(), Vec3(-L * 0.5f, 0.0f, -W * 0.5f), Vec3(L * 0.5f, 0.12f, W * 0.5f), m_Def.label_color);
            AppendBoxMesh(mesh.get(), Vec3(-L * 0.42f, 0.12f, -0.06f), Vec3(-L * 0.36f, H, 0.06f), m_Def.label_color);
            AppendBoxMesh(mesh.get(), Vec3(L * 0.36f, 0.12f, -0.06f), Vec3(L * 0.42f, H, 0.06f), m_Def.label_color);
            for (int p = 0; p < 5; ++p) {
                float pz = 0.09f + p * 0.045f;
                AppendBoxMesh(mesh.get(), Vec3(-L * 0.48f, 0.14f, pz), Vec3(L * 0.48f, H * 0.96f, pz + 0.02f), m_Def.base_color);
            }
            break;
        }
        case CargoID::CORRUGATED_ROOF_SHEETS: {
            // Solid extruded sinusoidal corrugated roofing sheet pack
            uint32_t segZ = 40;
            float sheetT = 0.012f;
            for (int layer = 0; layer < 4; ++layer) {
                float yBase = layer * (H * 0.24f);
                uint32_t topBase = static_cast<uint32_t>(mesh->vertices.size());
                for (int ix = 0; ix <= 1; ++ix) {
                    float x = (ix == 0) ? -L * 0.5f : L * 0.5f;
                    for (uint32_t iz = 0; iz <= segZ; ++iz) {
                        float v = static_cast<float>(iz) / static_cast<float>(segZ);
                        float z = -W * 0.5f + v * W;
                        float wave = 0.022f * std::sin(v * 8.0f * 6.2831853f);
                        Vertex vert;
                        vert.position = Vec3(x, yBase + wave + 0.025f, z);
                        vert.uv[0] = static_cast<float>(ix); vert.uv[1] = v;
                        vert.color = m_Def.base_color;
                        mesh->vertices.push_back(vert);
                    }
                }
                uint32_t botBase = static_cast<uint32_t>(mesh->vertices.size());
                for (int ix = 0; ix <= 1; ++ix) {
                    float x = (ix == 0) ? -L * 0.5f : L * 0.5f;
                    for (uint32_t iz = 0; iz <= segZ; ++iz) {
                        float v = static_cast<float>(iz) / static_cast<float>(segZ);
                        float z = -W * 0.5f + v * W;
                        float wave = 0.022f * std::sin(v * 8.0f * 6.2831853f);
                        Vertex vert;
                        vert.position = Vec3(x, yBase + wave + 0.025f - sheetT, z);
                        vert.uv[0] = static_cast<float>(ix); vert.uv[1] = v;
                        vert.color = m_Def.base_color * 0.85f;
                        mesh->vertices.push_back(vert);
                    }
                }
                uint32_t stride = segZ + 1;
                for (uint32_t iz = 0; iz < segZ; ++iz) {
                    // Top (+Y)
                    mesh->indices.push_back(topBase + iz);
                    mesh->indices.push_back(topBase + iz + 1);
                    mesh->indices.push_back(topBase + stride + iz);
                    mesh->indices.push_back(topBase + iz + 1);
                    mesh->indices.push_back(topBase + stride + iz + 1);
                    mesh->indices.push_back(topBase + stride + iz);
                    // Bottom (-Y)
                    mesh->indices.push_back(botBase + iz);
                    mesh->indices.push_back(botBase + stride + iz);
                    mesh->indices.push_back(botBase + iz + 1);
                    mesh->indices.push_back(botBase + iz + 1);
                    mesh->indices.push_back(botBase + stride + iz);
                    mesh->indices.push_back(botBase + stride + iz + 1);
                    // -X edge wall
                    mesh->indices.push_back(topBase + iz);
                    mesh->indices.push_back(botBase + iz);
                    mesh->indices.push_back(topBase + iz + 1);
                    mesh->indices.push_back(topBase + iz + 1);
                    mesh->indices.push_back(botBase + iz);
                    mesh->indices.push_back(botBase + iz + 1);
                    // +X edge wall
                    mesh->indices.push_back(topBase + stride + iz);
                    mesh->indices.push_back(topBase + stride + iz + 1);
                    mesh->indices.push_back(botBase + stride + iz);
                    mesh->indices.push_back(topBase + stride + iz + 1);
                    mesh->indices.push_back(botBase + stride + iz + 1);
                    mesh->indices.push_back(botBase + stride + iz);
                }
            }
            break;
        }
        default: {
            // CMU Pallet, Precast Concrete Slab, Gypsum Drywall Stack, Roof Tile Pallet
            AppendBoxMesh(mesh.get(), Vec3(-L * 0.5f, 0.0f, -W * 0.5f), Vec3(L * 0.5f, H, W * 0.5f), m_Def.base_color);
            break;
        }
    }

    mesh->CalculateSmoothNormals();
    mesh->CalculateTangents();
    mesh->CalculateBounds();

    m_CachedMesh = mesh;
    m_Bounds = mesh->bounds;
    return mesh;
}

bool BuildingMaterialsCargo::GenerateTextures(VkDevice device, VkQueue queue, VkCommandPool pool) {
    m_IsGenerated = true;
    return true;
}

} // namespace sphere::cargo
