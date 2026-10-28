#include "../include/WaterSurfaceMaterial.h"
#include "../textures/spec/water_surface_spec.h"

namespace sphere {

WaterSurfaceMaterial::WaterSurfaceMaterial(WaterVariant variant) {
    SetVariant(variant);
}

void WaterSurfaceMaterial::SetVariant(WaterVariant variant) {
    m_Variant = variant;
    m_Def.id = MaterialID::WATER_SURFACE;
    m_Def.name = "Ocean & Pool Water Surface";
    m_Def.category = "Optical & Refractive";
    m_Def.surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;
    m_Def.transmission = 0.92f;
    m_Def.ior = WaterSurfaceTextureSpec::WaterIor; // 1.333
    m_Def.metallic = 0.0f;
    m_Def.roughness = 0.05f;
    m_Def.dispersion = 0.008f;
    m_Def.normalScale = 1.0f;

    switch (variant) {
        case WaterVariant::OCEAN_DEEP:
            m_Def.baseColorFactor = Vec4{0.06f, 0.28f, 0.38f, 0.50f};
            m_Def.absorptionColor = Vec3{0.04f, 0.32f, 0.45f};
            m_Def.absorptionDensity = 0.4f;
            break;
        case WaterVariant::CLEAR_POOL:
            m_Def.baseColorFactor = Vec4{0.15f, 0.55f, 0.65f, 0.30f};
            m_Def.absorptionColor = Vec3{0.12f, 0.60f, 0.70f};
            m_Def.absorptionDensity = 0.15f;
            break;
        case WaterVariant::RIVER_RAPIDS:
            m_Def.baseColorFactor = Vec4{0.20f, 0.35f, 0.32f, 0.60f};
            m_Def.roughness = 0.18f;
            m_Def.normalScale = 2.2f;
            break;
        case WaterVariant::RAIN_PUDDLE:
            m_Def.baseColorFactor = Vec4{0.12f, 0.16f, 0.18f, 0.70f};
            m_Def.roughness = 0.02f;
            m_Def.normalScale = 0.2f;
            break;
    }
}

bool WaterSurfaceMaterial::GenerateTextures(GeneratorContext& context) {
    m_TextureIndices[static_cast<size_t>(MaterialChannel::Albedo)] = 501;
    m_TextureIndices[static_cast<size_t>(MaterialChannel::Normal)] = 502;
    m_TextureIndices[static_cast<size_t>(MaterialChannel::Refraction)] = 503;
    m_IsGenerated = true;
    return true;
}

uint32_t WaterSurfaceMaterial::GetTextureIndex(MaterialChannel channel) const {
    auto idx = static_cast<size_t>(channel);
    if (idx < m_TextureIndices.size()) {
        return m_TextureIndices[idx];
    }
    return 0;
}

} // namespace sphere
