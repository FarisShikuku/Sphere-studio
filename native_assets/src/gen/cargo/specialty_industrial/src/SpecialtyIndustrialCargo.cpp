#include "SpecialtyIndustrialCargo.h"
#include <cmath>
#include <algorithm>

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
        // Parametric Torus Equation with radial tread block harmonics:
        // P(theta, phi) = ((R_major + r(theta, phi)*cos(phi))*cos(theta), y_k + 0.68*r*sin(phi), (R_major + r*cos(phi))*sin(theta))
        int numTyres = 4;
        float outerR = m_Def.dimensions.x * 0.5f;
        float minorR = outerR * 0.26f;
        float majorR = outerR - minorR;
        float tyreH = m_Def.dimensions.z / static_cast<float>(numTyres);

        uint32_t uSegs = 32;
        uint32_t vSegs = 16;

        for (int t = 0; t < numTyres; ++t) {
            float yCenter = (t + 0.5f) * tyreH;
            uint32_t baseVert = static_cast<uint32_t>(mesh->vertices.size());

            for (uint32_t iv = 0; iv <= vSegs; ++iv) {
                float phi = (static_cast<float>(iv) / static_cast<float>(vSegs)) * 6.2831853f;
                float cp = std::cos(phi);
                float sp = std::sin(phi);

                for (uint32_t iu = 0; iu <= uSegs; ++iu) {
                    float theta = (static_cast<float>(iu) / static_cast<float>(uSegs)) * 6.2831853f;
                    // Tread blocks concentrated on outer crown (cp > 0.3)
                    float tread = (cp > 0.3f) ? (0.014f * (cp - 0.3f) * std::cos(24.0f * theta)) : 0.0f;
                    float rTube = minorR + tread;
                    float R = majorR + rTube * cp;

                    Vertex vert;
                    vert.position = Vec3(R * std::cos(theta), yCenter + 0.68f * rTube * sp, R * std::sin(theta));
                    vert.uv[0] = static_cast<float>(iu) / static_cast<float>(uSegs);
                    vert.uv[1] = static_cast<float>(iv) / static_cast<float>(vSegs);
                    vert.color = m_Def.base_color;
                    mesh->vertices.push_back(vert);
                }
            }

            for (uint32_t iv = 0; iv < vSegs; ++iv) {
                for (uint32_t iu = 0; iu < uSegs; ++iu) {
                    uint32_t r0 = baseVert + iv * (uSegs + 1);
                    uint32_t r1 = baseVert + (iv + 1) * (uSegs + 1);
                    mesh->indices.push_back(r0 + iu); mesh->indices.push_back(r1 + iu); mesh->indices.push_back(r0 + iu + 1);
                    mesh->indices.push_back(r0 + iu + 1); mesh->indices.push_back(r1 + iu); mesh->indices.push_back(r1 + iu + 1);
                }
            }
        }
    } else if (m_Def.id == CargoID::STEEL_PIPE_6M) {
        // Triangularly stacked 3-pipe pyramid with protective end rings
        uint32_t lenSegs = 28, radSegs = 20;
        float halfL = m_Def.dimensions.x * 0.5f;
        float r = m_Def.dimensions.y * 0.25f;
        Vec3 offsets[3] = { Vec3(0, r, -r * 1.05f), Vec3(0, r, r * 1.05f), Vec3(0, r * 2.82f, 0) };

        for (int p = 0; p < 3; ++p) {
            uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
            for (uint32_t il = 0; il <= lenSegs; ++il) {
                float u = static_cast<float>(il) / static_cast<float>(lenSegs);
                float x = -halfL + u * (2.0f * halfL);
                for (uint32_t ir = 0; ir <= radSegs; ++ir) {
                    float v = static_cast<float>(ir) / static_cast<float>(radSegs);
                    float angle = v * 6.2831853f;
                    Vertex vert;
                    vert.position = offsets[p] + Vec3(x, std::cos(angle) * r, std::sin(angle) * r);
                    vert.uv[0] = u; vert.uv[1] = v;
                    vert.color = (u < 0.03f || u > 0.97f) ? m_Def.label_color : m_Def.base_color;
                    mesh->vertices.push_back(vert);
                }
            }
            for (uint32_t il = 0; il < lenSegs; ++il) {
                for (uint32_t ir = 0; ir < radSegs; ++ir) {
                    uint32_t row0 = base + il * (radSegs + 1);
                    uint32_t row1 = base + (il + 1) * (radSegs + 1);
                    mesh->indices.push_back(row0 + ir); mesh->indices.push_back(row0 + ir + 1); mesh->indices.push_back(row1 + ir);
                    mesh->indices.push_back(row0 + ir + 1); mesh->indices.push_back(row1 + ir + 1); mesh->indices.push_back(row1 + ir);
                }
            }
            uint32_t frontRing = base;
            uint32_t backRing = base + lenSegs * (radSegs + 1);
            Vertex fc{}; fc.position = offsets[p] + Vec3(-halfL, 0.0f, 0.0f); fc.uv[0] = 0.0f; fc.uv[1] = 0.5f; fc.color = m_Def.label_color;
            Vertex bc{}; bc.position = offsets[p] + Vec3(halfL, 0.0f, 0.0f); bc.uv[0] = 1.0f; bc.uv[1] = 0.5f; bc.color = m_Def.label_color;
            uint32_t fcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(fc);
            uint32_t bcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(bc);
            for (uint32_t ir = 0; ir < radSegs; ++ir) {
                mesh->indices.push_back(fcIdx); mesh->indices.push_back(frontRing + ir + 1); mesh->indices.push_back(frontRing + ir);
                mesh->indices.push_back(bcIdx); mesh->indices.push_back(backRing + ir); mesh->indices.push_back(backRing + ir + 1);
            }
        }
    } else {
        // Masonry brick pallet cube or industrial cement sack
        float halfL = m_Def.dimensions.x * 0.5f;
        float halfW = m_Def.dimensions.y * 0.5f;
        float H = m_Def.dimensions.z;

        uint32_t base = static_cast<uint32_t>(mesh->vertices.size());
        Vec3 corners[8] = {
            Vec3(-halfL, 0.0f, -halfW), Vec3(halfL, 0.0f, -halfW),
            Vec3(halfL, H, -halfW),     Vec3(-halfL, H, -halfW),
            Vec3(-halfL, 0.0f, halfW),  Vec3(halfL, 0.0f, halfW),
            Vec3(halfL, H, halfW),      Vec3(-halfL, H, halfW)
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
            0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7, 0, 1, 5, 0, 5, 4,
            3, 6, 2, 3, 7, 6, 0, 4, 7, 0, 7, 3, 1, 2, 6, 1, 6, 5
        };
        for (int i = 0; i < 36; ++i) mesh->indices.push_back(base + cubeIndices[i]);
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
