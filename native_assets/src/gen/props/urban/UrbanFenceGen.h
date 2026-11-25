#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct UrbanFenceParams {
    float length = 4.0f;       // Length in meters (2.0 to 12.0)
    float height = 2.0f;       // Post height (1.0 to 3.5)
    float postSpacing = 2.0f;  // Distance between posts (1.0 to 3.0)
    int fenceStyle = 0;        // 0: Chain-Link Security, 1: Wrought-Iron Spear, 2: Pedestrian Guardrail, 3: Modern Slat, 4: Construction Mesh
    int colorScheme = 0;       // 0: Galvanized Steel, 1: Powder Jet Black, 2: Safety High-Vis Yellow, 3: Architectural Teak/Charcoal
    bool hasSecurityTop = true;// Top barbed/razor wire, spearhead cresting, hazard tape, or diagonal kickers
};

class UrbanFenceGen : public IGenerator {
public:
    UrbanFenceGen();
    ~UrbanFenceGen() override = default;

    const char* getName() const override { return "UrbanFenceGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const UrbanFenceParams& params) { params_ = params; }
    const UrbanFenceParams& getParams() const { return params_; }

private:
    UrbanFenceParams params_;
};

} // namespace procedural::props
