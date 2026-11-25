#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct FarmWagonParams {
    float length = 3.6f;          // Bed length (2.2 to 5.0)
    float width = 1.6f;           // Bed width (1.1 to 2.2)
    float wheelDiameter = 1.1f;   // Wheel diameter (0.7 to 1.5)
    int wagonStyle = 0;           // 0: High Stake Hay Rack, 1: Flatbed Utility Wagon, 2: Low Grain Box
    int woodFinish = 0;           // 0: Weathered Oak, 1: Barn Red, 2: Harvest Green
    bool hasDrawbarSeat = true;   // Front drawbar hitch tongue & spring driver bench
};

class FarmWagonGen : public IGenerator {
public:
    FarmWagonGen();
    ~FarmWagonGen() override = default;

    const char* getName() const override { return "FarmWagonGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const FarmWagonParams& params) { params_ = params; }
    const FarmWagonParams& getParams() const { return params_; }

private:
    FarmWagonParams params_;
};

} // namespace procedural::props
