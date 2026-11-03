#pragma once
#include "../../IMaterial.h"
#include <array>
namespace sphere {
enum class DirtVariant : uint32_t {
    EARTH_WITH_FOLIAGE = 0, // 700mm topsoil with organic groundcover
    COMPACTED_DRY_SOIL,
    WET_MUD_CLAY,
    GRAVEL_DIRT_TRACK
};
class DirtMaterial : public IMaterial {
public:
    explicit DirtMaterial(DirtVariant variant = DirtVariant::EARTH_WITH_FOLIAGE);
    ~DirtMaterial() override = default;
    MaterialID GetID() const override { return MaterialID::EARTH_FOLIAGE; }
    const char* GetName() const override { return "Earth with Foliage (700mm)"; }
    const char* GetCategory() const override { return "Nature & Ground Surfaces"; }
    const MaterialDef& GetDef() const override { return m_Def; }
    void SetDef(const MaterialDef& def) override { m_Def = def; }
    bool GenerateTextures(GeneratorContext& context) override;
    uint32_t GetTextureIndex(MaterialChannel channel) const override;
    bool IsGenerated() const override { return m_IsGenerated; }
    void SetVariant(DirtVariant variant);
    DirtVariant GetVariant() const { return m_Variant; }
private:
    DirtVariant m_Variant = DirtVariant::EARTH_WITH_FOLIAGE;
    MaterialDef m_Def;
    bool m_IsGenerated = false;
    std::array<uint32_t, static_cast<size_t>(MaterialChannel::COUNT)> m_TextureIndices{};
};
} // namespace spher
