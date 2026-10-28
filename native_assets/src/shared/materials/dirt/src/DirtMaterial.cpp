#include "../include/DirtMaterial.h"
#include "../textures/spec/dirt_spec.h"

namespace sphere {

DirtMaterial::DirtMaterial(DirtVariant variant) {
    SetVariant(variant);
}

void DirtMaterial::SetVariant(DirtVariant variant) {
    m_Variant = variant;
    m_Def.id = MaterialID::EARTH_FOLIAGE;
    m_Def.name = "Earth with Foliage (700mm)";
    m_Def.category = "Nature & Ground Surfaces";
    m_Def.tileScaleMeters = DirtTextureSpec::TileSizeMeters;
    m_Def.surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;
    m_Def.roughness = 0.90f;
    m_Def.metallic = 0.0f;
    m_Def.occlusionStrength = 1.0f;
    m_Def.subsurface = 0.22f;
    m_Def.subsurfaceColor = Vec3{0.45f, 0.55f, 0.15f};
    m_Def.normalScale = 1.15f;
    m_Def.displacementDepth = 0.032f;

    switch (variant) {
        case DirtVariant::EARTH_WITH_FOLIAGE:
            m_Def.baseColorFactor = Vec4{0.42f, 0.32f, 0.21f, 1.0f};
            break;
        case DirtVariant::COMPACTED_DRY_SOIL:
            m_Def.baseColorFactor = Vec4{0.52f, 0.42f, 0.30f, 1.0f};
            m_Def.roughness = 0.95f;
            m_Def.subsurface = 0.05f;
            break;
        case DirtVariant::WET_MUD_CLAY:
            m_Def.baseColorFactor = Vec4{0.25f, 0.18f, 0.12f, 1.0f};
            m_Def.roughness = 0.35f;
            break;
        case DirtVariant::GRAVEL_DIRT_TRACK:
            m_Def.baseColorFactor = Vec4{0.48f, 0.44f, 0.38f, 1.0f};
            m_Def.normalScale = 1.8f;
            break;
    }
}

bool DirtMaterial::GenerateTextures(GeneratorContext& context) {
    m_TextureIndices[static_cast<size_t>(MaterialChannel::Albedo)] = 301;
    m_TextureIndices[static_cast<size_t>(MaterialChannel::Normal)] = 302;
    m_TextureIndices[static_cast<size_t>(MaterialChannel::ORM)] = 303;
    m_TextureIndices[static_cast<size_t>(MaterialChannel::Height)] = 304;
    m_IsGenerated = true;
    return true;
}

uint32_t DirtMaterial::GetTextureIndex(MaterialChannel channel) const {
    auto idx = static_cast<size_t>(channel);
    if (idx < m_TextureIndices.size()) {
        return m_TextureIndices[idx];
    }
    return 0;
}

} // namespace sphere
