#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include <string>

namespace procedural::props {

struct ModernGateParams {
    float width = 4.6f;        // customParam0: Opening/total span width (m)
    float height = 2.2f;       // customParam1: Leaf/portal height (m)
    float postDepth = 0.40f;   // customParam2: Pillar depth (m)
    int style = 0;             // customParam3: Pattern/design sub-variant
    int colorScheme = 0;       // customParam4: Finish/tint index
    bool hasSmartHardware = true; // customParam5: Keypad, LED strips, actuators, lighting
};

#define DECLARE_MODERN_GATE_GEN(ClassName, GenNameStr)                     \
class ClassName : public IGenerator {                                      \
public:                                                                    \
    ClassName();                                                           \
    ~ClassName() override = default;                                       \
    const char* getName() const override { return GenNameStr; }              \
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; } \
    bool initialize(const GeneratorContext& ctx) override;                 \
    void update(float timeSeconds, float dt) override;                     \
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override; \
    void configure(const ModernGateParams& params) { params_ = params; }   \
    const ModernGateParams& getParams() const { return params_; }          \
private:                                                                   \
    ModernGateParams resolveParams(const GeneratorContext& ctx) const;     \
    ModernGateParams params_;                                              \
};

DECLARE_MODERN_GATE_GEN(ModernSlatGateGen, "ModernSlatGateGen")
DECLARE_MODERN_GATE_GEN(GlassPivotGateGen, "GlassPivotGateGen")
DECLARE_MODERN_GATE_GEN(LaserArtGateGen, "LaserArtGateGen")
DECLARE_MODERN_GATE_GEN(BifoldSpeedGateGen, "BifoldSpeedGateGen")
DECLARE_MODERN_GATE_GEN(AluminumLouverGateGen, "AluminumLouverGateGen")
DECLARE_MODERN_GATE_GEN(BrutalistPortalGateGen, "BrutalistPortalGateGen")
DECLARE_MODERN_GATE_GEN(GlassSpeedGateGen, "GlassSpeedGateGen")
DECLARE_MODERN_GATE_GEN(JapaneseToriGateGen, "JapaneseToriGateGen")
DECLARE_MODERN_GATE_GEN(GabionTimberGateGen, "GabionTimberGateGen")
DECLARE_MODERN_GATE_GEN(SolarSmartGateGen, "SolarSmartGateGen")

#undef DECLARE_MODERN_GATE_GEN

} // namespace procedural::props
