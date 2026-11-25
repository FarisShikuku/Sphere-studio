#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct FenceParams {
    float length = 4.0f;           // Total length in meters (2.0 to 10.0)
    float height = 1.2f;           // Post height (0.8 to 2.2)
    float postSpacing = 2.0f;      // Distance between vertical posts
    int fenceType = 0;             // 0: Split-Rail Ranch, 1: White Picket, 2: Barbed-Wire Cattle, 3: Dry-Stack Stone Wall, 4: Post-and-Board Horse, 5: Electric Livestock
    int railCount = 3;             // Horizontal rails / wire strands (2 to 5)
    int woodTint = 0;              // 0: Weathered Cedar, 1: Crisp White, 2: Dark Creosote, 3: Natural Oak, 4: Fieldstone
    bool hasAccentDetail = true;   // Diagonal H-brace, post caps, or warning signs
};

class FenceGen : public IGenerator {
public:
    FenceGen();
    ~FenceGen() override = default;

    const char* getName() const override { return "FenceGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const FenceParams& params) { params_ = params; }
    const FenceParams& getParams() const { return params_; }

private:
    FenceParams params_;
};

} // namespace procedural::props
