#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct FarmGateParams {
    float width = 3.6f;         // Width in meters (2.0 to 6.0)
    float height = 1.2f;        // Height (1.0 to 2.0)
    float postThickness = 0.15f;// Post thickness (0.12 to 0.28)
    int gateStyle = 0;          // 0: 5-Bar Tubular Steel, 1: Timber Paddock Gate, 2: Arched Bow Gate, 3: Texas Cattle Guard
    int colorScheme = 0;        // 0: Weathered Wood/Galvanized, 1: Painted White, 2: Barn Red, 3: Forest Green
    bool hasLockChain = true;   // Padlock chain & heavy latch post
};

class FarmGateGen : public IGenerator {
public:
    FarmGateGen();
    ~FarmGateGen() override = default;

    const char* getName() const override { return "FarmGateGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const FarmGateParams& params) { params_ = params; }
    const FarmGateParams& getParams() const { return params_; }

private:
    FarmGateParams params_;
};

} // namespace procedural::props
