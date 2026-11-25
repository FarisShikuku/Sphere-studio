#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct CrateParams {
    float width = 1.0f;           // Width in meters
    float height = 1.0f;          // Height in meters
    float depth = 1.0f;           // Depth in meters
    bool hasCrossBraces = true;   // Diagonal X braces on faces
    bool hasMetalCorners = true;  // Steel reinforcement brackets
    int woodStyle = 0;            // 0: Pine Shipping Crate, 1: Military Olive, 2: Dark Hardwood
};

class CrateGen : public IGenerator {
public:
    CrateGen();
    ~CrateGen() override = default;

    const char* getName() const override { return "CrateGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const CrateParams& params) { params_ = params; }
    const CrateParams& getParams() const { return params_; }

private:
    CrateParams params_;
};

} // namespace procedural::props
