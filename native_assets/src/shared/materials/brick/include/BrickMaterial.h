#pragma once
#include "../../IMaterial.h"
#include "BrickVariants.h"
#include <array>
namespace sphere {
class BrickMaterial : public IMaterial {
public:
    explicit BrickMaterial(BrickVariant variant = BrickVariant::RED_RUSTICATED);
    ~BrickMaterial() override = default;
    MaterialID GetID() const override;
    const char* GetName() const override;
    const char* GetCategory() const override { return "Masonry & Architecture"; }
    const MaterialDef& GetDef() const override { return m_Def; }
    void SetDef(const MaterialDef& def) override { m_Def = def; }
    bool GenerateTextures(GeneratorContext& context) override;
    uint32_t GetTextureIndex(MaterialChannel channel) const override;
    bool IsGenerated() const override { return m_IsGenerated; }
    void SetVariant(BrickVariant variant);
    BrickVariant GetVariant() const { return m_Variant; }
private:
    BrickVariant m_Variant = BrickVariant::RED_RUSTICATED;
    MaterialDef m_Def;
    bool m_IsGenerated = false;
    std::array<uint32_t, static_cast<size_t>(MaterialChannel::COUNT)> m_TextureIndices{};
};
} // namespace spher
