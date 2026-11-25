#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct BollardParams {
    float height = 1.0f;       // Height (0.6 to 1.5)
    float radius = 0.12f;      // Radius (0.06 to 0.25)
    float spacing = 1.5f;      // Spacing if multi-bollard array
    int style = 0;             // 0: Stainless Steel Ram, 1: Heritage Cast Iron Fluted, 2: Hydraulic Retractable, 3: Flexible Rebounder, 4: Illuminated Path
    int bollardCount = 3;      // Number in row (1 to 5)
    bool hasConnectingChains = true; // Connecting heavy steel chains linking bollards
};

class BollardGen : public IGenerator {
public:
    BollardGen();
    ~BollardGen() override = default;

    const char* getName() const override { return "BollardGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const BollardParams& params) { params_ = params; }
    const BollardParams& getParams() const { return params_; }

private:
    BollardParams params_;
};

} // namespace procedural::props
