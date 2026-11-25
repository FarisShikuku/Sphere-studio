#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct HydrantParams {
    float height = 0.85f;          // Height in meters (0.6 to 1.1)
    float barrelRadius = 0.14f;    // Barrel radius (0.10 to 0.20)
    int outletCount = 3;           // 2: Two 2.5" side nozzles, 3: Two side + 1 front 4.5" pumper nozzle
    int colorScheme = 0;           // 0: Classic Fire Red, 1: High-Vis Yellow, 2: Silver/Chrome
    bool hasChains = true;         // Hanging cap retaining chains
    bool hasPentagonNut = true;    // Operating stem nut on bonnet
};

class HydrantGen : public IGenerator {
public:
    HydrantGen();
    ~HydrantGen() override = default;

    const char* getName() const override { return "HydrantGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const HydrantParams& params) { params_ = params; }
    const HydrantParams& getParams() const { return params_; }

private:
    HydrantParams params_;
};

} // namespace procedural::props
