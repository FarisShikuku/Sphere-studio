#pragma once
#include "../../IMaterial.h"
#include <array>

namespace sphere {

enum class FoliageVariant : uint32_t {
    DECIDUOUS_OAK = 0,
    CONIFER_NEEDLE,
    BROADLEAF_IVY,
    FERN_FROND
};

class FoliageMaterial : public IMaterial {
public:
    explicit FoliageMaterial(FoliageVariant variant = FoliageVariant::DECIDUOUS_OAK);
    ~FoliageMaterial() override = default;

    MaterialID GetID() const override { return MaterialID::FOLIAGE_LEAF; }
    const char* GetName() const override { return "Foliage & Leaves (Subsurface)"; }
    const char* GetCategory() const override { return "Nature & Ground Surfaces"; }
    const MaterialDef& GetDef() const override { return m_Def; }
    void SetDef(const MaterialDef& def) override { m_Def = def; }

    bool GenerateTextures(GeneratorContext& context) override;
    uint32_t GetTextureIndex(MaterialChannel channel) const override;
    bool IsGenerated() const override { return m_IsGenerated; }

    void SetVariant(FoliageVariant variant);
    FoliageVariant GetVariant() const { return m_Variant; }

private:
    FoliageVariant m_Variant = FoliageVariant::DECIDUOUS_OAK;
    MaterialDef m_Def;
    bool m_IsGenerated = false;
    std::array<uint32_t, static_cast<size_t>(MaterialChannel::COUNT)> m_TextureIndices{};
};

} // namespace sphere
