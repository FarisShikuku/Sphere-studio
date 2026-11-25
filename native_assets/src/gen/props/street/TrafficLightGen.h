#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct TrafficLightParams {
    float mastHeight = 5.5f;       // Mast height
    float armLength = 4.0f;        // Horizontal arm over intersection
    int activeLight = 2;           // 0: Red, 1: Yellow, 2: Green
    bool hasPedestrianBox = true;
    bool hasSecondaryHead = true;
    int housingColor = 0;          // 0: Safety Yellow, 1: Matte Black, 2: Dark Green
};

class TrafficLightGen : public IGenerator {
public:
    TrafficLightGen();
    ~TrafficLightGen() override = default;

    const char* getName() const override { return "TrafficLightGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const TrafficLightParams& params) { params_ = params; }
    const TrafficLightParams& getParams() const { return params_; }

private:
    TrafficLightParams params_;
};

} // namespace procedural::props
