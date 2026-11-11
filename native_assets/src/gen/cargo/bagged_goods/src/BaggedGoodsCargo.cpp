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

    uint32_t segX = (lod == 0) ? 24 : 16;
    uint32_t segY = (lod == 0) ? 18 : 12;

    float halfL = m_Def.dimensions.x * 0.5f;
    float halfW = m_Def.dimensions.y * 0.5f;
    float halfH = m_Def.dimensions.z * 0.5f;
    float wrinkleAmp = m_Def.wrinkle_or_corrugation * 0.04f;

    // We generate top surface and bottom surface stitched along the seam edges
    // Top sheet (z > 0) and Bottom sheet (z < 0)
    for (int side = 0; side < 2; ++side) {
        float zSign = (side == 0) ? 1.0f : -1.0f;
        uint32_t baseIdx = static_cast<uint32_t>(mesh->vertices.size());

        for (uint32_t iy = 0; iy <= segY; ++iy) {
            float v = static_cast<float>(iy) / static_cast<float>(segY);
            float y = -halfW + v * (2.0f * halfW);
            float ny = (halfW > 1e-4f) ? (y / halfW) : 0.0f;
            float profileY = std::max(0.0f, 1.0f - std::pow(ny, 4.0f));

            for (uint32_t ix = 0; ix <= segX; ++ix) {
                float u = static_cast<float>(ix) / static_cast<float>(segX);
                float x = -halfL + u * (2.0f * halfL);
                float nx = (halfL > 1e-4f) ? (x / halfL) : 0.0f;
                float profileX = std::max(0.0f, 1.0f - std::pow(nx, 4.0f));

                // Pillow inflation formula with fold ripples
                float bulge = std::sqrt(std::max(0.0f, profileX * profileY));
                float wrinkle = std::sin(nx * 14.0f + ny * 8.0f) * std::cos(ny * 12.0f) * wrinkleAmp * bulge;

                float z = zSign * (bulge * halfH + wrinkle);

                // Slight tapering and sewn corner pinch
                float pinch = 1.0f - 0.15f * (nx * nx * ny * ny);
                x *= pinch;
                y *= pinch;

                Vertex vert;
                vert.position = Vec3(x, z, y); // Y is up in SphereStudio coordinate convention
                vert.uv[0] = u;
                vert.uv[1] = (side == 0) ? v * 0.5f : 0.5f + v * 0.5f;
                vert.color = m_Def.base_color;
                mesh->vertices.push_back(vert);
            }
        }

        // Triangles for this side
        for (uint32_t iy = 0; iy < segY; ++iy) {
            for (uint32_t ix = 0; ix < segX; ++ix) {
                uint32_t row0 = baseIdx + iy * (segX + 1);
                uint32_t row1 = baseIdx + (iy + 1) * (segX + 1);

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
