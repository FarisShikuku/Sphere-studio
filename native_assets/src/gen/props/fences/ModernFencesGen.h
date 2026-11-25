#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include <string>

namespace procedural::props {

struct ModernFenceParams {
    float length = 4.8f;       // customParam0: Span length (m)
    float height = 1.9f;       // customParam1: Fence height (m)
    float postSpacing = 1.6f;  // customParam2: Post bay spacing (m)
    int style = 0;             // customParam3: Pattern/design sub-variant
    int colorScheme = 0;       // customParam4: Finish/tint index
    bool hasAccentDetail = true; // customParam5: Top rails, lighting, or botanicals
};

#define DECLARE_MODERN_FENCE_GEN(ClassName, GenNameStr)                    \
class ClassName : public IGenerator {                                      \
public:                                                                    \
    ClassName();                                                           \
    ~ClassName() override = default;                                       \
    const char* getName() const override { return GenNameStr; }              \
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; } \
    bool initialize(const GeneratorContext& ctx) override;                 \
    void update(float timeSeconds, float dt) override;                     \
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override; \
    void configure(const ModernFenceParams& params) { params_ = params; }  \
    const ModernFenceParams& getParams() const { return params_; }         \
private:                                                                   \
    ModernFenceParams resolveParams(const GeneratorContext& ctx) const;    \
    ModernFenceParams params_;                                             \
};

DECLARE_MODERN_FENCE_GEN(ModernGlassFenceGen, "ModernGlassFenceGen")
DECLARE_MODERN_FENCE_GEN(AluminumFinFenceGen, "AluminumFinFenceGen")
DECLARE_MODERN_FENCE_GEN(LaserCutFenceGen, "LaserCutFenceGen")
DECLARE_MODERN_FENCE_GEN(GabionStoneFenceGen, "GabionStoneFenceGen")
DECLARE_MODERN_FENCE_GEN(ConcreteRibFenceGen, "ConcreteRibFenceGen")
DECLARE_MODERN_FENCE_GEN(LouverBladeFenceGen, "LouverBladeFenceGen")
DECLARE_MODERN_FENCE_GEN(LivingWallFenceGen, "LivingWallFenceGen")
DECLARE_MODERN_FENCE_GEN(BambooSteelFenceGen, "BambooSteelFenceGen")
DECLARE_MODERN_FENCE_GEN(CableRailFenceGen, "CableRailFenceGen")
DECLARE_MODERN_FENCE_GEN(CorrugatedCortenFenceGen, "CorrugatedCortenFenceGen")

#undef DECLARE_MODERN_FENCE_GEN

} // namespace procedural::props
