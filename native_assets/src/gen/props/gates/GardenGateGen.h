#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct GardenGateParams {
    float arborWidth = 1.6f;    // Arbor width (1.0 to 2.4m)
    float arborHeight = 2.4f;   // Arbor height (1.8 to 3.2m)
    int style = 0;              // 0: Rose Arbor Pergola, 1: Cottage Wicket, 2: Zen Moon Gate, 3: Iron Floral Arch
    int colorScheme = 0;        // 0: Cedar Wood, 1: Cottage White, 2: Slate Charcoal, 3: Wrought Iron Black
    bool hasLantern = true;     // Hanging carriage lantern / thumb latch
};

class GardenGateGen : public IGenerator {
public:
    GardenGateGen();
    ~GardenGateGen() override = default;

    const char* getName() const override { return "GardenGateGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const GardenGateParams& params) { params_ = params; }
    const GardenGateParams& getParams() const { return params_; }

private:
    GardenGateParams params_;
};

} // namespace procedural::props
