#pragma once
#include "../../IMaterial.h"
#include "GrassSurfaceVariants.h"
#include <array>
namespace sphere {
class GrassSurfaceMaterial : public IMaterial {
public:
    explicit GrassSurfaceMaterial(GrassSurfaceVariant variant = GrassSurfaceVariant::MONDO_LAWN_SURFACE);
    ~GrassSurfaceMaterial() override = default;
    MaterialID GetID() const override { return MaterialID::GRASS_SURFACE; }
    const char* GetName() const override { return "Mondo Grass (Surface Texture)"; }
    const char* GetCategory() const override { return "Nature & Ground Surfaces"; }
    const MaterialDef& GetDef() const override { return m_Def; }
    void SetDef(const MaterialDef& def) override { m_Def = def; }
    bool GenerateTextures(GeneratorContext& context) override;
    uint32_t GetTextureIndex(MaterialChannel channel) const override;
    bool IsGenerated() const override { return m_IsGenerated; }
    void SetVariant(GrassSurfaceVariant variant);
    GrassSurfaceVariant GetVariant() const { return m_Variant; }
private:
    GrassSurfaceVariant m_Variant = GrassSurfaceVariant::MONDO_LAWN_SURFACE;
    MaterialDef m_Def;
    bool m_IsGenerated = false;
    std::array<uint32_t, static_cast<size_t>(MaterialChannel::COUNT)> m_TextureIndices{};
};
} // namespace spher
