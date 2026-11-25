#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct GrainSiloParams {
    float totalHeight = 8.0f;     // Total height in meters (5.0 to 18.0)
    float diameter = 4.5f;        // Silo bin diameter (3.0 to 9.0)
    float wallHeight = 5.5f;      // Cylinder wall height (3.5 to 14.0)
    int ringTiers = 5;            // Corrugated ring count (3 to 10)
    int finishType = 0;           // 0: Galvanized Zinc, 1: Farm Barn Red, 2: Silo White
    bool hasCageLadder = true;    // Safety caged exterior inspection ladder & auger
};

class GrainSiloGen : public IGenerator {
public:
    GrainSiloGen();
    ~GrainSiloGen() override = default;

    const char* getName() const override { return "GrainSiloGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const GrainSiloParams& params) { params_ = params; }
    const GrainSiloParams& getParams() const { return params_; }

private:
    GrainSiloParams params_;
};

} // namespace procedural::props
