#include "../include/GlassMaterial.h"
#include "../textures/spec/glass_spec.h"
namespace sphere {
GlassMaterial::GlassMaterial(GlassVariant variant) {
    SetVariant(variant);
}
void GlassMaterial::SetVariant(GlassVariant variant) {
    m_Variant = variant;
    m_Def.id = MaterialID::GLASS_CLEAR;
    m_Def.name = "Refractive Architectural Glass";
    m_Def.category = "Optical & Refractive";
    m_Def.surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;
    m_Def.transmission = 0.98f;
    m_Def.ior = GlassTextureSpec::DefaultIor; // 1.52
    m_Def.dispersion = 0.015f; // Chromatic aberration
    m_Def.metallic = 0.0f;
    m_Def.roughness = 0.03f;
    m_Def.absorptionColor = Vec3{0.88f, 0.96f, 0.94f};
    m_Def.absorptionDensity = 0.1f;
    switch (variant) {
        case GlassVariant::CLEAR_ARCHITECTURAL:
            m_Def.baseColorFactor = Vec4{0.95f, 0.97f, 1.0f, 0.05f};
            break;
        case GlassVariant::FROSTED_ETCHED:
            m_Def.baseColorFactor = Vec4{0.90f, 0.95f, 0.98f, 0.20f};
            m_Def.roughness = 0.40f;
            m_Def.refractionRoughness = 0.35f;
            break;
        case GlassVariant::TINTED_BRONZE:
            m_Def.baseColorFactor = Vec4{0.45f, 0.35f, 0.22f, 0.45f};
            m_Def.absorptionColor = Vec3{0.35f, 0.25f, 0.15f};
            m_Def.absorptionDensity = 0.8f;
            break;
        case GlassVariant::SAFETY_LAMINATED:
            m_Def.baseColorFactor = Vec4{0.92f, 0.94f, 0.96f, 0.08f};
            m_Def.dispersion = 0.025f;
            break;
    }
}
bool GlassMaterial::GenerateTextures(GeneratorContext& context) {
    m_TextureIndices[static_cast<size_t>(MaterialChannel::Refraction)] = 401;
    m_TextureIndices[static_cast<size_t>(MaterialChannel::Albedo)] = 402;
    m_IsGenerated = true;
    return true;
}
uint32_t GlassMaterial::GetTextureIndex(MaterialChannel channel) const {
    auto idx = static_cast<size_t>(channel);
    if (idx < m_TextureIndices.size()) {
        return m_TextureIndices[idx];
    }
    return 0;
}
} // namespace spher
