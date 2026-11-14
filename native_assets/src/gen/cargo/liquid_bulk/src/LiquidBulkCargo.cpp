#include "LiquidBulkCargo.h"
#include <cmath>
#include <algorithm>

namespace sphere::cargo {

LiquidBulkCargo::LiquidBulkCargo(CargoID variantId) {
    const LiquidVariantConfig* cfg = LiquidBulkVariants::FindVariant(variantId);
    if (cfg) {
        m_Def = LiquidBulkVariants::CreateCargoDef(*cfg);
    } else {
        m_Def.id = variantId;
        m_Def.name = GetCargoName(variantId);
        m_Def.category = "Liquid Bulk";
        m_Def.packingType = CargoPackingType::DRUM;
    }
}

std::shared_ptr<Mesh> LiquidBulkCargo::GenerateMesh(uint32_t lod) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    uint32_t radialSegments = (lod == 0) ? 36 : 24;
    uint32_t heightSegments = (lod == 0) ? 36 : 20;

    if (m_Def.id == CargoID::WATER_TANKER) {
        // Horizontal elliptical-head pressure vessel capsule along X
        float halfCyl = m_Def.dimensions.x * 0.36f;
        float radius = m_Def.dimensions.y * 0.5f;
        uint32_t lenSegs = 32;

        for (uint32_t il = 0; il <= lenSegs; ++il) {
            float u = static_cast<float>(il) / static_cast<float>(lenSegs);
            float x = 0.0f, r = radius;
            if (u < 0.2f) {
                float t = (0.2f - u) / 0.2f;
                x = -halfCyl - std::sin(t * 1.570796f) * (radius * 0.65f);
                r = radius * std::cos(t * 1.570796f);
            } else if (u > 0.8f) {
                float t = (u - 0.8f) / 0.2f;
                x = halfCyl + std::sin(t * 1.570796f) * (radius * 0.65f);
                r = radius * std::cos(t * 1.570796f);
            } else {
                float t = (u - 0.2f) / 0.6f;
                x = -halfCyl + t * (2.0f * halfCyl);
                // Baffle seam ribs at 1/3 and 2/3
                float rib = std::exp(-std::pow((t - 0.33f) * 30.0f, 2.0f)) * 0.025f +
                            std::exp(-std::pow((t - 0.67f) * 30.0f, 2.0f)) * 0.025f;
                r += rib;
            }

            for (uint32_t ir = 0; ir <= radialSegments; ++ir) {
                float v = static_cast<float>(ir) / static_cast<float>(radialSegments);
                float angle = v * 6.2831853f;
                Vertex vert;
                vert.position = Vec3(x, std::cos(angle) * r + radius, std::sin(angle) * r);
                vert.uv[0] = u;
                vert.uv[1] = v;
                vert.color = m_Def.base_color;
                mesh->vertices.push_back(vert);
            }
        }

        for (uint32_t il = 0; il < lenSegs; ++il) {
            for (uint32_t ir = 0; ir < radialSegments; ++ir) {
                uint32_t r0 = il * (radialSegments + 1);
                uint32_t r1 = (il + 1) * (radialSegments + 1);
                mesh->indices.push_back(r0 + ir);
                mesh->indices.push_back(r0 + ir + 1);
                mesh->indices.push_back(r1 + ir);
                mesh->indices.push_back(r0 + ir + 1);
                mesh->indices.push_back(r1 + ir + 1);
                mesh->indices.push_back(r1 + ir);
            }
        }
        // Closed front (-X) and rear (+X) end cap discs
        uint32_t frontRing = 0;
        uint32_t backRing = lenSegs * (radialSegments + 1);
        Vertex fc{}; fc.position = Vec3(-halfCyl - radius * 0.65f, radius, 0.0f); fc.uv[0] = 0.0f; fc.uv[1] = 0.5f; fc.color = m_Def.base_color;
        Vertex bc{}; bc.position = Vec3(halfCyl + radius * 0.65f, radius, 0.0f); bc.uv[0] = 1.0f; bc.uv[1] = 0.5f; bc.color = m_Def.base_color;
        uint32_t fcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(fc);
        uint32_t bcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(bc);
        for (uint32_t ir = 0; ir < radialSegments; ++ir) {
            mesh->indices.push_back(fcIdx); mesh->indices.push_back(frontRing + ir + 1); mesh->indices.push_back(frontRing + ir);
            mesh->indices.push_back(bcIdx); mesh->indices.push_back(backRing + ir); mesh->indices.push_back(backRing + ir + 1);
        }
    } else if (m_Def.id == CargoID::JERRICAN_20L) {
        // Superquadric rectangular canister with X-stiffening recess & top spout bevel
        float hw = m_Def.dimensions.x * 0.5f;
        float hd = m_Def.dimensions.y * 0.5f;
        float H = m_Def.dimensions.z;

        for (uint32_t iy = 0; iy <= heightSegments; ++iy) {
            float v = static_cast<float>(iy) / static_cast<float>(heightSegments);
            float y = v * H;
            float taper = (v > 0.78f) ? (1.0f - (v - 0.78f) * 1.15f) : 1.0f;

            for (uint32_t ix = 0; ix <= radialSegments; ++ix) {
                float u = static_cast<float>(ix) / static_cast<float>(radialSegments);
                float theta = u * 6.2831853f;
                float ct = std::cos(theta);
                float st = std::sin(theta);

                // Superellipse (p = 6.0) cross-section
                float superR = std::pow(std::pow(std::abs(ct), 6.0f) + std::pow(std::abs(st), 6.0f), -1.0f / 6.0f);
                // Side X-stiffening indent pattern on +-Z walls
                float xEmb = (std::abs(st) > 0.75f && v > 0.15f && v < 0.75f)
                    ? (-0.010f * std::exp(-std::pow(std::abs(ct) - std::abs(v - 0.45f) * 2.2f, 2.0f) * 20.0f))
                    : 0.0f;

                float x = ct * superR * hw * taper;
                float z = (st * superR * hd + (st > 0 ? xEmb : -xEmb)) * taper;

                Vertex vert;
                vert.position = Vec3(x, y, z);
                vert.uv[0] = u;
                vert.uv[1] = v;
                vert.color = (v > 0.90f) ? m_Def.label_color : m_Def.base_color;
                mesh->vertices.push_back(vert);
            }
        }

        for (uint32_t iy = 0; iy < heightSegments; ++iy) {
            for (uint32_t ix = 0; ix < radialSegments; ++ix) {
                uint32_t r0 = iy * (radialSegments + 1);
                uint32_t r1 = (iy + 1) * (radialSegments + 1);
                mesh->indices.push_back(r0 + ix);
                mesh->indices.push_back(r1 + ix);
                mesh->indices.push_back(r0 + ix + 1);
                mesh->indices.push_back(r0 + ix + 1);
                mesh->indices.push_back(r1 + ix);
                mesh->indices.push_back(r1 + ix + 1);
            }
        }
        // Closed bottom (-Y) & top (+Y) caps
        uint32_t botRing = 0;
        uint32_t topRing = heightSegments * (radialSegments + 1);
        Vertex bc{}; bc.position = Vec3(0.0f, 0.0f, 0.0f); bc.uv[0] = 0.5f; bc.uv[1] = 0.0f; bc.color = m_Def.base_color * 0.85f;
        Vertex tc{}; tc.position = Vec3(0.0f, H, 0.0f); tc.uv[0] = 0.5f; tc.uv[1] = 1.0f; tc.color = m_Def.label_color;
        uint32_t bcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(bc);
        uint32_t tcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(tc);
        for (uint32_t ix = 0; ix < radialSegments; ++ix) {
            mesh->indices.push_back(bcIdx); mesh->indices.push_back(botRing + ix); mesh->indices.push_back(botRing + ix + 1);
            mesh->indices.push_back(tcIdx); mesh->indices.push_back(topRing + ix + 1); mesh->indices.push_back(topRing + ix);
        }
    } else {
        // Revolution vessels: OIL_DRUM_200L, MILK_CHURN, GAS_CYLINDER
        float radius = m_Def.dimensions.x * 0.5f;
        float H = m_Def.dimensions.z;

        for (uint32_t iy = 0; iy <= heightSegments; ++iy) {
            float v = static_cast<float>(iy) / static_cast<float>(heightSegments);
            float y = v * H;
            float r = radius;

            if (m_Def.id == CargoID::OIL_DRUM_200L) {
                // Gaussian twin rolling hoops + chime beads
                float hoop1 = std::exp(-std::pow((v - 0.34f) / 0.025f, 2.0f)) * 0.018f;
                float hoop2 = std::exp(-std::pow((v - 0.66f) / 0.025f, 2.0f)) * 0.018f;
                float chimeT = std::exp(-std::pow((v - 0.98f) / 0.018f, 2.0f)) * 0.014f;
                float chimeB = std::exp(-std::pow((v - 0.02f) / 0.018f, 2.0f)) * 0.014f;
                r += hoop1 + hoop2 + chimeT + chimeB;
            } else if (m_Def.id == CargoID::GAS_CYLINDER) {
                // Torispherical dome (v > 0.68) + protective valve collar shroud (v > 0.85) + foot skirt (v < 0.08)
                if (v < 0.08f) {
                    r = radius * 0.82f;
                } else if (v > 0.68f && v <= 0.88f) {
                    float domeT = (v - 0.68f) / 0.20f;
                    r = radius * std::pow(std::max(0.05f, 1.0f - domeT * domeT), 0.45f);
                } else if (v > 0.88f) {
                    // Protective shroud collar
                    r = radius * 0.48f;
                }
            } else if (m_Def.id == CargoID::MILK_CHURN) {
                // Conical breast neck & mushroom lid
                if (v > 0.62f && v <= 0.84f) {
                    float t = (v - 0.62f) / 0.22f;
                    r = radius * (1.0f - 0.45f * t);
                } else if (v > 0.84f && v <= 0.92f) {
                    r = radius * 0.55f;
                } else if (v > 0.92f) {
                    float capT = (v - 0.92f) / 0.08f;
                    r = radius * 0.64f * std::sqrt(std::max(0.05f, 1.0f - capT * capT));
                }
            }

            for (uint32_t ix = 0; ix <= radialSegments; ++ix) {
                float u = static_cast<float>(ix) / static_cast<float>(radialSegments);
                float angle = u * 6.2831853f;

                Vertex vert;
                vert.position = Vec3(std::cos(angle) * r, y, std::sin(angle) * r);
                vert.uv[0] = u;
                vert.uv[1] = v;
                vert.color = m_Def.base_color;
                mesh->vertices.push_back(vert);
            }
        }

        for (uint32_t iy = 0; iy < heightSegments; ++iy) {
            for (uint32_t ix = 0; ix < radialSegments; ++ix) {
                uint32_t row0 = iy * (radialSegments + 1);
                uint32_t row1 = (iy + 1) * (radialSegments + 1);
                mesh->indices.push_back(row0 + ix);
                mesh->indices.push_back(row1 + ix);
                mesh->indices.push_back(row0 + ix + 1);
                mesh->indices.push_back(row0 + ix + 1);
                mesh->indices.push_back(row1 + ix);
                mesh->indices.push_back(row1 + ix + 1);
            }
        }
        // Closed bottom (-Y) & top (+Y) caps
        uint32_t botRing = 0;
        uint32_t topRing = heightSegments * (radialSegments + 1);
        Vertex bc{}; bc.position = Vec3(0.0f, 0.0f, 0.0f); bc.uv[0] = 0.5f; bc.uv[1] = 0.0f; bc.color = m_Def.base_color * 0.85f;
        Vertex tc{}; tc.position = Vec3(0.0f, H, 0.0f); tc.uv[0] = 0.5f; tc.uv[1] = 1.0f; tc.color = m_Def.base_color * 0.95f;
        uint32_t bcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(bc);
        uint32_t tcIdx = static_cast<uint32_t>(mesh->vertices.size()); mesh->vertices.push_back(tc);
        for (uint32_t ix = 0; ix < radialSegments; ++ix) {
            mesh->indices.push_back(bcIdx); mesh->indices.push_back(botRing + ix); mesh->indices.push_back(botRing + ix + 1);
            mesh->indices.push_back(tcIdx); mesh->indices.push_back(topRing + ix + 1); mesh->indices.push_back(topRing + ix);
        }
    }

    mesh->CalculateSmoothNormals();
    mesh->CalculateTangents();
    mesh->CalculateBounds();

    m_CachedMesh = mesh;
    m_Bounds = mesh->bounds;
    return mesh;
}

bool LiquidBulkCargo::GenerateTextures(VkDevice device, VkQueue queue, VkCommandPool pool) {
    m_IsGenerated = true;
    return true;
}

} // namespace sphere::cargo
