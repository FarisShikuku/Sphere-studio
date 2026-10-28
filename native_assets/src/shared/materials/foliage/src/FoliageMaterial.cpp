#include "../include/FoliageMaterial.h"
#include "../textures/spec/foliage_spec.h"

namespace sphere {

FoliageMaterial::FoliageMaterial(FoliageVariant variant) {
    SetVariant(variant);
}

void FoliageMaterial::SetVariant(FoliageVariant variant) {
    m_Variant = variant;
    m_Def.id = MaterialID::FOLIAGE_LEAF;
    m_Def.name = "Foliage & Leaves (Subsurface)";
    m_Def.category = "Nature & Ground Surfaces";
    m_Def.surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;
    m_Def.roughness = 0.55f;
    m_Def.metallic = 0.0f;
    m_Def.ior = 1.45f;
    m_Def.subsurface = 0.65f;
    m_Def.subsurfaceColor = Vec3{0.45f, 0.85f, 0.20f};
    m_Def.subsurfaceRadius = 2.0f; // mm

    switch (variant) {
        case FoliageVariant::DECIDUOUS_OAK:
            m_Def.baseColorFactor = Vec4{0.28f, 0.52f, 0.16f, 1.0f};
            break;
        case FoliageVariant::CONIFER_NEEDLE:
            m_Def.baseColorFactor = Vec4{0.14f, 0.32f, 0.15f, 1.0f};
            m_Def.subsurface = 0.30f;
            break;
        case FoliageVariant::BROADLEAF_IVY:
            m_Def.baseColorFactor = Vec4{0.20f, 0.46f, 0.12f, 1.0f};
            m_Def.subsurface = 0.75f;
            break;
        case FoliageVariant::FERN_FROND:
            m_Def.baseColorFactor = Vec4{0.32f, 0.60f, 0.18f, 1.0f};
            m_Def.subsurface = 0.80f;
            break;
    }
}

bool FoliageMaterial::GenerateTextures(GeneratorContext& context) {
    m_TextureIndices[static_cast<size_t>(MaterialChannel::Albedo)] = 601;
    m_TextureIndices[static_cast<size_t>(MaterialChannel::Normal)] = 602;
    m_TextureIndices[static_cast<size_t>(MaterialChannel::Subsurface)] = 603;
    m_IsGenerated = true;
    return true;
}

uint32_t FoliageMaterial::GetTextureIndex(MaterialChannel channel) const {
    auto idx = static_cast<size_t>(channel);
    if (idx < m_TextureIndices.size()) {
        return m_TextureIndices[idx];
    }
    return 0;
}

} // namespace sphere
