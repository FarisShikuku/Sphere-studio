#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct FlagPostParams {
    float poleHeight = 8.0f;       // Total flagpole height in meters (3.5 to 16.0)
    float flagLength = 2.4f;       // Kenyan flag fly length in meters (1.2 to 4.2, 3:2 ratio height)
    float poleRadius = 0.075f;     // Base pole shaft radius in meters (0.04 to 0.16)
    int mountStyle = 0;            // 0: Plaza Monument Plinth, 1: Ceremonial Circular Dais, 2: Sidewalk Flange
    int windMode = 1;              // 0: Gentle Breeze, 1: Moderate Wind, 2: Brisk Wind
    bool hasUplightsAndCleat = true;
};

class FlagPostGen : public IGenerator {
public:
    FlagPostGen();
    ~FlagPostGen() override = default;

    const char* getName() const override { return "FlagPostGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const FlagPostParams& params) { params_ = params; }
    const FlagPostParams& getParams() const { return params_; }

private:
    FlagPostParams params_;
};

} // namespace procedural::props
