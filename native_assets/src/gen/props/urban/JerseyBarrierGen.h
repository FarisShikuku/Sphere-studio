#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct JerseyBarrierParams {
    float length = 3.0f;       // Length in meters (1.5 to 6.0)
    float height = 0.85f;      // Height (0.6 to 1.2)
    float baseWidth = 0.60f;   // Base width (0.4 to 0.9)
    int barrierStyle = 0;      // 0: Concrete K-Rail, 1: Plastic Water-Filled, 2: Construction Barrier with Mesh Topper
    int colorScheme = 0;       // 0: Concrete Gray, 1: Safety Orange, 2: High-Vis Striped
    bool hasReflectors = true; // Top reflective delineators and base drainage scallops
};

class JerseyBarrierGen : public IGenerator {
public:
    JerseyBarrierGen();
    ~JerseyBarrierGen() override = default;

    const char* getName() const override { return "JerseyBarrierGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const JerseyBarrierParams& params) { params_ = params; }
    const JerseyBarrierParams& getParams() const { return params_; }

private:
    JerseyBarrierParams params_;
};

} // namespace procedural::props
