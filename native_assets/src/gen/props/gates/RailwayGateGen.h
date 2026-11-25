#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct RailwayGateParams {
    float armLength = 8.0f;     // Boom arm length (4.0 to 12.0m)
    float mastHeight = 4.2f;    // Mast height (3.0 to 5.5m)
    int style = 0;              // 0: Dual-Track Grade Crossing, 1: Cantilever Overhead Truss, 2: Pedestrian Maze Crossing
    int colorScheme = 0;        // 0: Standard, 1: High-Vis Yellow
    bool hasBellAndLights = true; // Top warning bell & 3 boom arm red warning lamps
};

class RailwayGateGen : public IGenerator {
public:
    RailwayGateGen();
    ~RailwayGateGen() override = default;

    const char* getName() const override { return "RailwayGateGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const RailwayGateParams& params) { params_ = params; }
    const RailwayGateParams& getParams() const { return params_; }

private:
    RailwayGateParams params_;
};

} // namespace procedural::props
