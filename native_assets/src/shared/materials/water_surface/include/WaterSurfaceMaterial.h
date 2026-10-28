#pragma once
#include "../../IMaterial.h"
#include <array>

namespace sphere {

enum class WaterVariant : uint32_t {
    OCEAN_DEEP = 0,
    CLEAR_POOL,
    RIVER_RAPIDS,
    RAIN_PUDDLE
};

class WaterSurfaceMaterial : public IMaterial {
public:
    explicit WaterSurfaceMaterial(WaterVariant variant = WaterVariant::OCEAN_DEEP);
    ~WaterSurfaceMaterial() override = default;

    MaterialID GetID() const override { return MaterialID::WATER_SURFACE; }
    const char* GetName() const override { return "Ocean & Pool Water Surface"; }
    const char* GetCategory() const override { return "Optical & Refractive"; }
    const MaterialDef& GetDef() const override { return m_Def; }
    void SetDef(const MaterialDef& def) override { m_Def = def; }

    bool GenerateTextures(GeneratorContext& context) override;
    uint32_t GetTextureIndex(MaterialChannel channel) const override;
    bool IsGenerated() const override { return m_IsGenerated; }

    void SetVariant(WaterVariant variant);
    WaterVariant GetVariant() const { return m_Variant; }

private:
    WaterVariant m_Variant = WaterVariant::OCEAN_DEEP;
    MaterialDef m_Def;
    bool m_IsGenerated = false;
    std::array<uint32_t, static_cast<size_t>(MaterialChannel::COUNT)> m_TextureIndices{};
};

} // namespace sphere
