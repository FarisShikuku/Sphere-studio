#include "../include/BrickMaterial.h"
#include "../textures/spec/brick_spec.h"
namespace sphere {
BrickMaterial::BrickMaterial(BrickVariant variant) {
    SetVariant(variant);
}
void BrickMaterial::SetVariant(BrickVariant variant) {
    m_Variant = variant;
    m_Def.id = (variant == BrickVariant::RED_RUSTICATED) ? MaterialID::BRICK_RUSTICATED : MaterialID::BRICK_RED;
    m_Def.name = "Rusticated Red Brick (900mm)";
    m_Def.category = "Masonry & Architecture";
    m_Def.tileScaleMeters = BrickTextureSpec::TileSizeMeters;
    m_Def.roughness = 0.85f;
    m_Def.metallic = 0.0f;
    m_Def.occlusionStrength = 1.0f;
    m_Def.normalScale = 1.25f;
    m_Def.displacementDepth = 0.028f;
    m_Def.surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;
    switch (variant) {
        case BrickVariant::RED_RUSTICATED:
            m_Def.baseColorFactor = Vec4{0.78f, 0.36f, 0.24f, 1.0f};
            break;
        case BrickVariant::GREY_ENGINEERING:
            m_Def.baseColorFactor = Vec4{0.55f, 0.55f, 0.58f, 1.0f};
            m_Def.roughness = 0.40f;
            break;
        case BrickVariant::WHITE_LIMEWASH:
            m_Def.baseColorFactor = Vec4{0.92f, 0.90f, 0.86f, 1.0f};
            m_Def.roughness = 0.90f;
            break;
        case BrickVariant::WEATHERED_RECLAIMED:
            m_Def.baseColorFactor = Vec4{0.68f, 0.32f, 0.20f, 1.0f};
            m_Def.normalScale = 1.6f;
            break;
        case BrickVariant::BLACK_BASALT:
            m_Def.baseColorFactor = Vec4{0.18f, 0.18f, 0.20f, 1.0f};
            break;
    }
}
MaterialID BrickMaterial::GetID() const {
    return m_Def.id;
}
const char* BrickMaterial::GetName() const {
    return m_Def.name.c_str();
}
bool BrickMaterial::GenerateTextures(GeneratorContext& context) {
    // In actual Vulkan runtime, dispatches brick_albedo.comp, brick_normal.comp, brick_orm.comp, brick_height.comp
    // and registers allocated VkImageViews with BindlessHeap.
    m_TextureIndices[static_cast<size_t>(MaterialChannel::Albedo)] = 101;
    m_TextureIndices[static_cast<size_t>(MaterialChannel::Normal)] = 102;
    m_TextureIndices[static_cast<size_t>(MaterialChannel::ORM)] = 103;
    m_TextureIndices[static_cast<size_t>(MaterialChannel::Height)] = 104;
    m_IsGenerated = true;
    return true;
}
uint32_t BrickMaterial::GetTextureIndex(MaterialChannel channel) const {
    auto idx = static_cast<size_t>(channel);
    if (idx < m_TextureIndices.size()) {
        return m_TextureIndices[idx];
    }
    return 0;
}
} // namespace spher
