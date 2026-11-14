#include "RawMaterialsCargo.h"
#include <cmath>
#include <algorithm>

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

static void AddPrism(Mesh& mesh, const Vec3& center, const Vec3& halfSize, const Vec4& col) {
    Vec3 p[8] = {
        center + Vec3(-halfSize.x, -halfSize.y, -halfSize.z),
        center + Vec3( halfSize.x, -halfSize.y, -halfSize.z),
        center + Vec3( halfSize.x,  halfSize.y, -halfSize.z),
        center + Vec3(-halfSize.x,  halfSize.y, -halfSize.z),
        center + Vec3(-halfSize.x, -halfSize.y,  halfSize.z),
        center + Vec3( halfSize.x, -halfSize.y,  halfSize.z),
        center + Vec3( halfSize.x,  halfSize.y,  halfSize.z),
        center + Vec3(-halfSize.x,  halfSize.y,  halfSize.z)
    };
    int faces[6][4] = {
        {4, 5, 6, 7}, {1, 0, 3, 2}, {3, 7, 6, 2},
        {0, 1, 5, 4}, {1, 2, 6, 5}, {0, 4, 7, 3}
    };
    Vec3 normals[6] = {
        {0, 0, 1}, {0, 0, -1}, {0, 1, 0},
        {0, -1, 0}, {1, 0, 0}, {-1, 0, 0}
    };
    for (int f = 0; f < 6; ++f) {
        uint32_t base = static_cast<uint32_t>(mesh.vertices.size());
        for (int c = 0; c < 4; ++c) {
            Vertex v;
            v.position = p[faces[f][c]];
            v.normal = normals[f];
            v.uv[0] = (c == 1 || c == 2) ? 1.0f : 0.0f;
            v.uv[1] = (c >= 2) ? 1.0f : 0.0f;
            v.color = col;
            mesh.vertices.push_back(v);
        }
        mesh.indices.push_back(base + 0);
        mesh.indices.push_back(base + 1);
        mesh.indices.push_back(base + 2);
        mesh.indices.push_back(base + 0);
        mesh.indices.push_back(base + 2);
        mesh.indices.push_back(base + 3);
    }
}

static void AddSplitWedgeBillet(
    Mesh& mesh, const Vec3& offset, float halfL, float radius,
    float angleStart, float angleSpan, const Vec4& barkCol, const Vec4& heartCol,
    uint32_t arcSegs = 10, uint32_t lenSegs = 12
) {
    // Parametric Wedge Billet: outer curved arc (angleStart..angleStart+angleSpan) + two flat radial split planes
    uint32_t baseArc = static_cast<uint32_t>(mesh.vertices.size());
    for (uint32_t il = 0; il <= lenSegs; ++il) {
        float u = static_cast<float>(il) / static_cast<float>(lenSegs);
        float x = -halfL + u * (2.0f * halfL);
        for (uint32_t ir = 0; ir <= arcSegs; ++ir) {
            float v = static_cast<float>(ir) / static_cast<float>(arcSegs);
            float theta = angleStart + v * angleSpan;
            float barkRipple = std::sin(theta * 14.0f + u * 5.0f) * 0.008f;
            float r = radius + barkRipple;
            Vertex vert;
            vert.position = offset + Vec3(x, std::sin(theta) * r, std::cos(theta) * r);
            vert.uv[0] = u;
            vert.uv[1] = v;
            vert.color = barkCol;
            mesh.vertices.push_back(vert);
        }
    }
    for (uint32_t il = 0; il < lenSegs; ++il) {
        for (uint32_t ir = 0; ir < arcSegs; ++ir) {
            uint32_t r0 = baseArc + il * (arcSegs + 1);
            uint32_t r1 = baseArc + (il + 1) * (arcSegs + 1);
            mesh.indices.push_back(r0 + ir);
            mesh.indices.push_back(r1 + ir);
            mesh.indices.push_back(r0 + ir + 1);
            mesh.indices.push_back(r0 + ir + 1);
            mesh.indices.push_back(r1 + ir);
            mesh.indices.push_back(r1 + ir + 1);
        }
    }

    // Two flat radial split faces (from pith center out to edges)
    float angles[2] = { angleStart, angleStart + angleSpan };
    for (int side = 0; side < 2; ++side) {
        float th = angles[side];
        Vec3 edgeDir(0.0f, std::sin(th) * radius, std::cos(th) * radius);
        uint32_t b = static_cast<uint32_t>(mesh.vertices.size());
        Vertex v0, v1, v2, v3;
        v0.position = offset + Vec3(-halfL, 0.0f, 0.0f); v0.color = heartCol;
        v1.position = offset + Vec3( halfL, 0.0f, 0.0f); v1.color = heartCol;
        v2.position = offset + Vec3( halfL, 0.0f, 0.0f) + edgeDir; v2.color = heartCol * 0.92f;
        v3.position = offset + Vec3(-halfL, 0.0f, 0.0f) + edgeDir; v3.color = heartCol * 0.92f;
        mesh.vertices.push_back(v0); mesh.vertices.push_back(v1);
        mesh.vertices.push_back(v2); mesh.vertices.push_back(v3);
        if (side == 0) {
            mesh.indices.push_back(b+0); mesh.indices.push_back(b+2); mesh.indices.push_back(b+1);
            mesh.indices.push_back(b+0); mesh.indices.push_back(b+3); mesh.indices.push_back(b+2);
        } else {
            mesh.indices.push_back(b+0); mesh.indices.push_back(b+1); mesh.indices.push_back(b+2);
            mesh.indices.push_back(b+0); mesh.indices.push_back(b+2); mesh.indices.push_back(b+3);
        }
    }

    // Closed heartwood end caps at x = -halfL (-X) and x = +halfL (+X)
    uint32_t frontCenterIdx = static_cast<uint32_t>(mesh.vertices.size());
    Vertex fc{}; fc.position = offset + Vec3(-halfL, 0.0f, 0.0f); fc.color = heartCol;
    mesh.vertices.push_back(fc);
    uint32_t backCenterIdx = static_cast<uint32_t>(mesh.vertices.size());
    Vertex bc{}; bc.position = offset + Vec3(halfL, 0.0f, 0.0f); bc.color = heartCol;
    mesh.vertices.push_back(bc);
    uint32_t frontArcBase = baseArc;
    uint32_t backArcBase = baseArc + lenSegs * (arcSegs + 1);
    for (uint32_t ir = 0; ir < arcSegs; ++ir) {
        mesh.indices.push_back(frontCenterIdx); mesh.indices.push_back(frontArcBase + ir); mesh.indices.push_back(frontArcBase + ir + 1);
        mesh.indices.push_back(backCenterIdx); mesh.indices.push_back(backArcBase + ir + 1); mesh.indices.push_back(backArcBase + ir);
    }
}

std::shared_ptr<Mesh> RawMaterialsCargo::GenerateMesh(uint32_t lod) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    if (m_Def.id == CargoID::COAL_PILE || m_Def.id == CargoID::GRAVEL_PILE) {
        // Angle-of-repose hyperbolic cone + 2D ridged Voronoi/simplex facet noise
        uint32_t rings = 24;
        uint32_t sectors = 32;
        float baseRadius = m_Def.dimensions.x * 0.5f;
        float peakHeight = m_Def.dimensions.z;

        for (uint32_t r = 0; r <= rings; ++r) {
            float fracR = static_cast<float>(r) / static_cast<float>(rings);
            float currentRadius = fracR * baseRadius;
            float slope = std::pow(std::max(0.0f, 1.0f - fracR), 1.25f) * peakHeight;

            for (uint32_t s = 0; s <= sectors; ++s) {
                float fracS = static_cast<float>(s) / static_cast<float>(sectors);
                float angle = fracS * 6.2831853f;
                float x = std::cos(angle) * currentRadius;
                float z = std::sin(angle) * currentRadius;

                float ridgeNoise = std::abs(std::sin(angle * 7.0f + fracR * 10.0f) *
                                            std::cos(fracR * 16.0f - angle * 3.0f)) * 0.045f * (1.0f - fracR * 0.5f);
                float y = slope + ridgeNoise;

                Vertex vert;
                vert.position = Vec3(x, y, z);
                vert.uv[0] = 0.5f + (x / (baseRadius * 2.2f));
                vert.uv[1] = 0.5f + (z / (baseRadius * 2.2f));
                vert.color = m_Def.base_color * (0.85f + ridgeNoise * 4.0f);
                mesh->vertices.push_back(vert);
            }
        }

        for (uint32_t r = 0; r < rings; ++r) {
            for (uint32_t s = 0; s < sectors; ++s) {
                uint32_t r0 = r * (sectors + 1);
                uint32_t r1 = (r + 1) * (sectors + 1);
                mesh->indices.push_back(r0 + s);
                mesh->indices.push_back(r0 + s + 1);
                mesh->indices.push_back(r1 + s);
                mesh->indices.push_back(r0 + s + 1);
                mesh->indices.push_back(r1 + s + 1);
                mesh->indices.push_back(r1 + s);
            }
        }
        // Closed bottom disc (-Y)
        uint32_t outerRing = rings * (sectors + 1);
        Vertex bc{}; bc.position = Vec3(0.0f, 0.0f, 0.0f); bc.uv[0] = 0.5f; bc.uv[1] = 0.5f; bc.color = m_Def.base_color * 0.7f;
        uint32_t bcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(bc);
        for (uint32_t s = 0; s < sectors; ++s) {
            mesh->indices.push_back(bcIdx); mesh->indices.push_back(outerRing + s); mesh->indices.push_back(outerRing + s + 1);
        }
    } else if (m_Def.id == CargoID::TIMBER_PLANK) {
        // 3x4 grid of precision milled rectangular lumber boards + spacer stickers
        float halfL = m_Def.dimensions.x * 0.5f;
        int cols = 3, rows = 4;
        float boardW = (m_Def.dimensions.y * 0.92f) / cols;
        float boardH = (m_Def.dimensions.z * 0.85f) / rows;
        for (int r = 0; r < rows; ++r) {
            float cy = (r + 0.5f) * (m_Def.dimensions.z / rows);
            for (int c = 0; c < cols; ++c) {
                float cz = -m_Def.dimensions.y * 0.5f + (c + 0.5f) * (m_Def.dimensions.y / cols);
                Vec4 tint = m_Def.base_color * (0.94f + 0.06f * ((r + c) % 2));
                AddPrism(*mesh, Vec3(0, cy, cz), Vec3(halfL, boardH * 0.44f, boardW * 0.45f), tint);
            }
        }
    } else if (m_Def.id == CargoID::CHOPPED_FIREWOOD_SPLIT) {
        // Quarter-Split Oak Firewood Wedges (3 clustered radial 90-degree sectors)
        float halfL = m_Def.dimensions.x * 0.5f;
        float rad = m_Def.dimensions.y * 0.45f;
        AddSplitWedgeBillet(*mesh, Vec3(0.0f, 0.02f, -0.06f), halfL, rad, 0.15f, 1.35f, m_Def.base_color, m_Def.label_color);
        AddSplitWedgeBillet(*mesh, Vec3(0.01f, 0.02f, 0.08f), halfL * 0.96f, rad * 0.94f, 1.45f, 1.40f, m_Def.base_color * 0.92f, m_Def.label_color * 0.95f);
        AddSplitWedgeBillet(*mesh, Vec3(-0.01f, rad * 0.72f, 0.01f), halfL * 0.94f, rad * 0.90f, 0.40f, 1.45f, m_Def.base_color * 1.04f, m_Def.label_color);
    } else if (m_Def.id == CargoID::HALF_ROUND_FIREWOOD) {
        // Half-Split Silver Birch Logs (two semicircular 180-deg logs stacked)
        float halfL = m_Def.dimensions.x * 0.5f;
        float rad = m_Def.dimensions.y * 0.42f;
        AddSplitWedgeBillet(*mesh, Vec3(0.0f, 0.02f, -rad * 0.65f), halfL, rad, 0.0f, 3.14159f, m_Def.base_color, m_Def.label_color, 14, 12);
        AddSplitWedgeBillet(*mesh, Vec3(0.02f, 0.02f, rad * 0.65f), halfL * 0.98f, rad * 0.95f, 0.0f, 3.14159f, m_Def.base_color * 0.95f, m_Def.label_color, 14, 12);
    } else if (m_Def.id == CargoID::STACKED_CORDWOOD_RICK) {
        // Cross-Stacked Cribbed Cordwood Rick: alternating split billets across 3 vertical layers
        float len = m_Def.dimensions.x;
        float wid = m_Def.dimensions.y;
        float hgt = m_Def.dimensions.z;
        for (int layer = 0; layer < 4; ++layer) {
            float y = (layer + 0.5f) * (hgt / 4.0f);
            for (int col = -2; col <= 2; ++col) {
                float xOff = col * (len * 0.19f);
                float rotShift = (layer % 2 == 0) ? 0.3f : 2.1f;
                AddSplitWedgeBillet(*mesh, Vec3(xOff, y, 0.0f), wid * 0.48f, hgt * 0.11f, rotShift, 1.7f,
                                    m_Def.base_color * (0.9f + 0.05f * (layer % 2)), m_Def.label_color, 8, 6);
            }
        }
    } else if (m_Def.id == CargoID::WOODEN_RAILWAY_SLEEPER) {
        // High-exponent superellipsoidal chamfered sleeper beam along X
        uint32_t lenSegs = 24, radSegs = 24;
        float halfL = m_Def.dimensions.x * 0.5f;
        float halfW = m_Def.dimensions.y * 0.5f;
        float halfH = m_Def.dimensions.z * 0.5f;
        for (uint32_t il = 0; il <= lenSegs; ++il) {
            float u = static_cast<float>(il) / static_cast<float>(lenSegs);
            float x = -halfL + u * (2.0f * halfL);
            for (uint32_t ir = 0; ir <= radSegs; ++ir) {
                float v = static_cast<float>(ir) / static_cast<float>(radSegs);
                float th = v * 6.2831853f;
                float ct = std::cos(th), st = std::sin(th);
                float s = std::pow(std::pow(std::abs(ct), 10.0f) + std::pow(std::abs(st), 10.0f), -0.1f);
                // Natural longitudinal seasoning check crack along top face
                float fissure = (st > 0.9f && std::abs(ct) < 0.08f) ? -0.012f : 0.0f;
                Vertex vert;
                vert.position = Vec3(x, (st * s * halfH) + halfH + fissure, ct * s * halfW);
                vert.uv[0] = u;
                vert.uv[1] = v;
                vert.color = (std::abs(u - 0.5f) > 0.47f) ? m_Def.label_color : m_Def.base_color;
                mesh->vertices.push_back(vert);
            }
        }
        for (uint32_t il = 0; il < lenSegs; ++il) {
            for (uint32_t ir = 0; ir < radSegs; ++ir) {
                uint32_t r0 = il * (radSegs + 1);
                uint32_t r1 = (il + 1) * (radSegs + 1);
                mesh->indices.push_back(r0 + ir); mesh->indices.push_back(r1 + ir); mesh->indices.push_back(r0 + ir + 1);
                mesh->indices.push_back(r0 + ir + 1); mesh->indices.push_back(r1 + ir); mesh->indices.push_back(r1 + ir + 1);
            }
        }
        // Closed sleeper ends (-X and +X)
        uint32_t frontRing = 0;
        uint32_t backRing = lenSegs * (radSegs + 1);
        Vertex fc{}; fc.position = Vec3(-halfL, halfH, 0.0f); fc.color = m_Def.label_color;
        Vertex bc{}; bc.position = Vec3(halfL, halfH, 0.0f); bc.color = m_Def.label_color;
        uint32_t fcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(fc);
        uint32_t bcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(bc);
        for (uint32_t ir = 0; ir < radSegs; ++ir) {
            mesh->indices.push_back(fcIdx); mesh->indices.push_back(frontRing + ir); mesh->indices.push_back(frontRing + ir + 1);
            mesh->indices.push_back(bcIdx); mesh->indices.push_back(backRing + ir + 1); mesh->indices.push_back(backRing + ir);
        }
    } else if (m_Def.id == CargoID::CHOPPED_KINDLING_BUNDLE) {
        // Cylindrical cluster of 7 hexagonal split kindling sticks with twine waist constriction
        float halfL = m_Def.dimensions.x * 0.5f;
        float stickR = m_Def.dimensions.y * 0.14f;
        for (int k = 0; k < 7; ++k) {
            float cy = m_Def.dimensions.z * 0.5f;
            float cz = 0.0f;
            if (k > 0) {
                float a = (k - 1) * (6.2831853f / 6.0f);
                cy += std::sin(a) * stickR * 1.95f;
                cz += std::cos(a) * stickR * 1.95f;
            }
            AddPrism(*mesh, Vec3(0.0f, cy, cz), Vec3(halfL, stickR * 0.85f, stickR * 0.85f),
                     m_Def.base_color * (0.92f + 0.08f * (k % 2)));
        }
    } else if (m_Def.id == CargoID::TIMBER_STUMP_BLOCKS) {
        // Flared root-buttress vertical trunk stump: R(v, theta) = R0 * (1 + 0.25*exp(-5v)*(1 + 0.45*cos(5*theta)))
        uint32_t hSegs = 20, radSegs = 32;
        float R0 = m_Def.dimensions.x * 0.38f;
        float H = m_Def.dimensions.z;
        for (uint32_t ih = 0; ih <= hSegs; ++ih) {
            float v = static_cast<float>(ih) / static_cast<float>(hSegs);
            float y = v * H;
            float rootFlare = 0.26f * std::exp(-5.2f * v);
            for (uint32_t ir = 0; ir <= radSegs; ++ir) {
                float u = static_cast<float>(ir) / static_cast<float>(radSegs);
                float th = u * 6.2831853f;
                float r = R0 * (1.0f + rootFlare * (1.0f + 0.45f * std::cos(5.0f * th)))
                             + 0.008f * std::sin(16.0f * th + v * 3.0f);
                Vertex vert;
                vert.position = Vec3(std::cos(th) * r, y, std::sin(th) * r);
                vert.uv[0] = u; vert.uv[1] = v;
                vert.color = (v > 0.96f) ? m_Def.label_color : m_Def.base_color;
                mesh->vertices.push_back(vert);
            }
        }
        for (uint32_t ih = 0; ih < hSegs; ++ih) {
            for (uint32_t ir = 0; ir < radSegs; ++ir) {
                uint32_t r0 = ih * (radSegs + 1);
                uint32_t r1 = (ih + 1) * (radSegs + 1);
                mesh->indices.push_back(r0 + ir); mesh->indices.push_back(r1 + ir); mesh->indices.push_back(r0 + ir + 1);
                mesh->indices.push_back(r0 + ir + 1); mesh->indices.push_back(r1 + ir); mesh->indices.push_back(r1 + ir + 1);
            }
        }
        // Closed bottom (-Y) & cut top (+Y) stump caps
        uint32_t botRing = 0;
        uint32_t topRing = hSegs * (radSegs + 1);
        Vertex bc{}; bc.position = Vec3(0.0f, 0.0f, 0.0f); bc.color = m_Def.base_color * 0.8f;
        Vertex tc{}; tc.position = Vec3(0.0f, H, 0.0f); tc.color = m_Def.label_color;
        uint32_t bcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(bc);
        uint32_t tcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(tc);
        for (uint32_t ir = 0; ir < radSegs; ++ir) {
            mesh->indices.push_back(bcIdx); mesh->indices.push_back(botRing + ir); mesh->indices.push_back(botRing + ir + 1);
            mesh->indices.push_back(tcIdx); mesh->indices.push_back(topRing + ir + 1); mesh->indices.push_back(topRing + ir);
        }
    } else {
        // Horizontal Oak Log or Hexagonal Deformed Steel Rebar Bundle (along X axis)
        uint32_t lenSegs = 28;
        uint32_t radSegs = 24;
        float halfL = m_Def.dimensions.x * 0.5f;
        float radius = m_Def.dimensions.y * 0.5f;

        for (uint32_t il = 0; il <= lenSegs; ++il) {
            float u = static_cast<float>(il) / static_cast<float>(lenSegs);
            float x = -halfL + u * (2.0f * halfL);
            float naturalTaper = 1.0f - 0.08f * u;

            for (uint32_t ir = 0; ir <= radSegs; ++ir) {
                float v = static_cast<float>(ir) / static_cast<float>(radSegs);
                float angle = v * 6.2831853f;

                float modR = (m_Def.id == CargoID::WOOD_LOG_OAK)
                    ? (0.015f * std::cos(14.0f * angle + 2.5f * std::sin(5.0f * u)) + 0.008f * std::sin(27.0f * angle - 9.0f * u))
                    : (0.006f * std::sin(20.0f * 3.14159f * u + 2.0f * angle)); // Rebar helical lugs
                float r = radius * naturalTaper + modR;

                Vertex vert;
                vert.position = Vec3(x, std::cos(angle) * r + radius, std::sin(angle) * r);
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
                mesh->indices.push_back(row0 + ir); mesh->indices.push_back(row0 + ir + 1); mesh->indices.push_back(row1 + ir);
                mesh->indices.push_back(row0 + ir + 1); mesh->indices.push_back(row1 + ir + 1); mesh->indices.push_back(row1 + ir);
            }
        }
        uint32_t frontRing = 0;
        uint32_t backRing = lenSegs * (radSegs + 1);
        Vertex fc{}; fc.position = Vec3(-halfL, radius, 0.0f); fc.color = m_Def.label_color;
        Vertex bc{}; bc.position = Vec3(halfL, radius, 0.0f); bc.color = m_Def.label_color;
        uint32_t fcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(fc);
        uint32_t bcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(bc);
        for (uint32_t ir = 0; ir < radSegs; ++ir) {
            mesh->indices.push_back(fcIdx); mesh->indices.push_back(frontRing + ir + 1); mesh->indices.push_back(frontRing + ir);
            mesh->indices.push_back(bcIdx); mesh->indices.push_back(backRing + ir); mesh->indices.push_back(backRing + ir + 1);
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
