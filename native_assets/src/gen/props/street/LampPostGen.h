#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct LampPostParams {
    float height = 6.0f;           // Total height in meters (4.0 to 10.0)
    float armReach = 1.8f;         // Outreach horizontal distance (0.5 to 3.0)
    float poleRadius = 0.12f;      // Base pole radius
    int luminaireStyle = 0;        // 0: Steel Cobra-head, 1: Vintage Lantern, 2: Minimalist LED Bar, 3: Acorn Globe, 4: Solar Smart LED, 5: High-Mast Floodlight
    bool doubleArm = false;        // Dual-sided street lamp
    float luminaireTiltDeg = 15.0f;
};

class LampPostGen : public IGenerator {
public:
    LampPostGen();
    ~LampPostGen() override = default;

    const char* getName() const override { return "LampPostGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const LampPostParams& params) { params_ = params; }
    const LampPostParams& getParams() const { return params_; }

private:
    LampPostParams params_;
};

} // namespace procedural::props
