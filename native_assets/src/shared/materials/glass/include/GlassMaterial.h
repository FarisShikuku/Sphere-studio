#pragma once
#include "../../IMaterial.h"
#include <array>

namespace sphere {

enum class GlassVariant : uint32_t {
    CLEAR_ARCHITECTURAL = 0,
    FROSTED_ETCHED,
    TINTED_BRONZE,
    SAFETY_LAMINATED
};

class GlassMaterial : public IMaterial {
public:
    explicit GlassMaterial(GlassVariant variant = GlassVariant::CLEAR_ARCHITECTURAL);
    ~GlassMaterial() override = default;

    MaterialID GetID() const override { return MaterialID::GLASS_CLEAR; }
    const char* GetName() const override { return "Refractive Architectural Glass"; }
    const char* GetCategory() const override { return "Optical & Refractive"; }
    const MaterialDef& GetDef() const override { return m_Def; }
    void SetDef(const MaterialDef& def) override { m_Def = def; }

    bool GenerateTextures(GeneratorContext& context) override;
    uint32_t GetTextureIndex(MaterialChannel channel) const override;
    bool IsGenerated() const override { return m_IsGenerated; }

    void SetVariant(GlassVariant variant);
    GlassVariant GetVariant() const { return m_Variant; }

private:
    GlassVariant m_Variant = GlassVariant::CLEAR_ARCHITECTURAL;
    MaterialDef m_Def;
    bool m_IsGenerated = false;
    std::array<uint32_t, static_cast<size_t>(MaterialChannel::COUNT)> m_TextureIndices{};
};

} // namespace sphere
