#include "BaggedGoodsCargo.h"
#include <cmath>
#include <algorithm>

namespace sphere::cargo {

BaggedGoodsCargo::BaggedGoodsCargo(CargoID variantId) {
    const BagVariantConfig* cfg = BaggedGoodsVariants::FindVariant(variantId);
    if (cfg) {
        m_Def = BaggedGoodsVariants::CreateCargoDef(*cfg);
    } else {
        m_Def.id = variantId;
        m_Def.name = GetCargoName(variantId);
        m_Def.category = "Bagged Goods";
        m_Def.packingType = CargoPackingType::BAG;
    }
}

std::shared_ptr<Mesh> BaggedGoodsCargo::GenerateMesh(uint32_t lod) {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = m_Def.name;
    mesh->topology = PrimitiveTopology::Triangles;

    uint32_t segX = (lod == 0) ? 28 : 16;
    uint32_t segZ = (lod == 0) ? 22 : 12;

    float halfL = m_Def.dimensions.x * 0.5f;
    float halfW = m_Def.dimensions.y * 0.5f;
    float halfH = m_Def.dimensions.z * 0.5f;
    float wrinkleAmp = m_Def.wrinkle_or_corrugation * 0.045f;

    // Mathematical Modeling: Superellipsoidal Catenary Pillow with Hermite Seam Collars
    // Phi(xi, eta) = ((1 - |xi|^2.6) * (1 - |eta|^2.2))^0.42
    // Pinch & Corner Ears: E(xi, eta) = 1 - 0.14*xi^2*eta^2 + 0.08*(1 - eta^2)*xi^4
    // Seam Ridge: S(xi) = 0.012 * exp(-42 * (|xi| - 0.94)^2)
    for (int side = 0; side < 2; ++side) {
        float ySign = (side == 0) ? 1.0f : -1.0f;
        uint32_t baseIdx = static_cast<uint32_t>(mesh->vertices.size());

        for (uint32_t iz = 0; iz <= segZ; ++iz) {
            float v = static_cast<float>(iz) / static_cast<float>(segZ);
            float eta = 2.0f * v - 1.0f;
            float envZ = std::max(0.0f, 1.0f - std::pow(std::abs(eta), 2.2f));

            for (uint32_t ix = 0; ix <= segX; ++ix) {
                float u = static_cast<float>(ix) / static_cast<float>(segX);
                float xi = 2.0f * u - 1.0f;
                float envX = std::max(0.0f, 1.0f - std::pow(std::abs(xi), 2.6f));

                float envelope = std::pow(std::max(0.0f, envX * envZ), 0.42f);

                // Lamé harmonic stress wrinkles radiating toward corners
                float wrinkle = (0.60f * std::sin(9.0f * xi * eta + 4.0f * xi) * std::cos(6.0f * eta) +
                                 0.40f * std::sin(15.0f * (xi * xi - eta * eta))) * wrinkleAmp * envelope;

                // Heat-sealed / stitched end flange inside perimeter, zero at outer seam boundary
                bool onBorder = (iz == 0 || iz == segZ || ix == 0 || ix == segX);
                float seamMask = onBorder ? 0.0f : std::min(1.0f, envelope * 6.0f);
                float seamFlange = seamMask * 0.014f * std::exp(-45.0f * std::pow(std::abs(xi) - 0.93f, 2.0f));

                // Gravity slump on bottom face, convex pillow dome on top face
                float gravityFactor = (side == 0) ? 1.08f : 0.92f;
                float y = onBorder ? 0.0f : ySign * (envelope * halfH * gravityFactor + wrinkle + seamFlange);

                // Gusseted corner ears: widen near corners (|xi|->1, |eta|->1), pinch waist slightly
                float gussetEar = 1.0f + 0.07f * std::pow(std::abs(xi), 4.0f) * std::pow(std::abs(eta), 2.0f)
                                       - 0.06f * (xi * xi) * (1.0f - eta * eta);

                float x = xi * halfL * gussetEar;
                float z = eta * halfW * (1.0f - 0.09f * xi * xi * (1.0f - eta * eta));

                Vertex vert;
                vert.position = Vec3(x, y + halfH, z);
                vert.uv[0] = u;
                vert.uv[1] = (side == 0) ? v * 0.5f : 0.5f + v * 0.5f;

                // Subtle stitched tape tint near the ends |xi| > 0.88
                if (std::abs(xi) > 0.88f) {
                    vert.color = m_Def.label_color * 0.85f + m_Def.base_color * 0.15f;
                } else {
                    vert.color = m_Def.base_color;
                }
                mesh->vertices.push_back(vert);
            }
        }

        for (uint32_t iz = 0; iz < segZ; ++iz) {
            for (uint32_t ix = 0; ix < segX; ++ix) {
                uint32_t row0 = baseIdx + iz * (segX + 1);
                uint32_t row1 = baseIdx + (iz + 1) * (segX + 1);

                uint32_t i0 = row0 + ix;
                uint32_t i1 = row0 + ix + 1;
                uint32_t i2 = row1 + ix;
                uint32_t i3 = row1 + ix + 1;

                if (side == 0) {
                    mesh->indices.push_back(i0);
                    mesh->indices.push_back(i2);
                    mesh->indices.push_back(i1);
                    mesh->indices.push_back(i1);
                    mesh->indices.push_back(i2);
                    mesh->indices.push_back(i3);
                } else {
                    mesh->indices.push_back(i0);
                    mesh->indices.push_back(i1);
                    mesh->indices.push_back(i2);
                    mesh->indices.push_back(i1);
                    mesh->indices.push_back(i3);
                    mesh->indices.push_back(i2);
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

bool BaggedGoodsCargo::GenerateTextures(VkDevice device, VkQueue queue, VkCommandPool pool) {
    m_IsGenerated = true;
    return true;
}

} // namespace sphere::cargo
