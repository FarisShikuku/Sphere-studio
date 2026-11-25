#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct PrivacyFenceParams {
    float length = 4.5f;        // Length in meters (2.0 to 12.0)
    float height = 2.0f;        // Height (1.5 to 4.0)
    float postSpacing = 1.8f;   // Post spacing (1.2 to 3.0)
    int fenceStyle = 0;         // 0: Concrete Acoustic Soundwall, 1: Cedar Tongue & Groove with Trellis, 2: Composite Slat, 3: Louvered Blade
    int colorScheme = 0;        // 0: Natural Cedar/Concrete, 1: Weathered Driftwood/Slate, 2: Dark Espresso/Charcoal, 3: Warm Desert Tan/Sandstone
    bool hasDecorativeCaps = true; // Decorative post caps & solar lights
};

class PrivacyFenceGen : public IGenerator {
public:
    PrivacyFenceGen();
    ~PrivacyFenceGen() override = default;

    const char* getName() const override { return "PrivacyFenceGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const PrivacyFenceParams& params) { params_ = params; }
    const PrivacyFenceParams& getParams() const { return params_; }

private:
    PrivacyFenceParams params_;
};

} // namespace procedural::props
