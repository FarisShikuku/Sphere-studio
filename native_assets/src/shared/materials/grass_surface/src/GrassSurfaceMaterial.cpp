#include "../include/GrassSurfaceMaterial.h"
#include "../textures/spec/grass_surface_spec.h"

namespace sphere {

GrassSurfaceMaterial::GrassSurfaceMaterial(GrassSurfaceVariant variant) {
    SetVariant(variant);
}

void GrassSurfaceMaterial::SetVariant(GrassSurfaceVariant variant) {
    m_Variant = variant;
    m_Def.id = MaterialID::GRASS_SURFACE;
    m_Def.name = "Mondo Grass (Surface Only)";
    m_Def.category = "Nature & Ground Surfaces";
    m_Def.tileScaleMeters = GrassSurfaceTextureSpec::TileSizeMeters;
    m_Def.surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE; // Surface-only (no geometry blades)
    m_Def.roughness = 0.72f;
    m_Def.metallic = 0.0f;
    m_Def.occlusionStrength = 0.9f;
    m_Def.subsurface = 0.35f;
    m_Def.subsurfaceColor = Vec3{0.35f, 0.58f, 0.18f};
    m_Def.normalScale = 0.85f;
    m_Def.displacementDepth = 0.012f;

    switch (variant) {
        case GrassSurfaceVariant::MONDO_LAWN_SURFACE:
            m_Def.baseColorFactor = Vec4{0.25f, 0.42f, 0.16f, 1.0f};
            break;
        case GrassSurfaceVariant::WILD_MEADOW_SURFACE:
            m_Def.baseColorFactor = Vec4{0.32f, 0.45f, 0.18f, 1.0f};
            m_Def.roughness = 0.80f;
            break;
        case GrassSurfaceVariant::DRY_SAVANNA_SURFACE:
            m_Def.baseColorFactor = Vec4{0.55f, 0.52f, 0.28f, 1.0f};
            m_Def.roughness = 0.88f;
            break;
        case GrassSurfaceVariant::WET_GOLF_TURF_SURFACE:
            m_Def.baseColorFactor = Vec4{0.18f, 0.48f, 0.14f, 1.0f};
            m_Def.roughness = 0.50f;
            break;
    }
}

bool GrassSurfaceMaterial::GenerateTextures(GeneratorContext& context) {
    m_TextureIndices[static_cast<size_t>(MaterialChannel::Albedo)] = 201;
    m_TextureIndices[static_cast<size_t>(MaterialChannel::Normal)] = 202;
    m_TextureIndices[static_cast<size_t>(MaterialChannel::ORM)] = 203;
    m_IsGenerated = true;
    return true;
}

uint32_t GrassSurfaceMaterial::GetTextureIndex(MaterialChannel channel) const {
    auto idx = static_cast<size_t>(channel);
    if (idx < m_TextureIndices.size()) {
        return m_TextureIndices[idx];
    }
    return 0;
}

} // namespace sphere
