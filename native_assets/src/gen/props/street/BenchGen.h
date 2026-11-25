#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct BenchParams {
    float length = 1.8f;           // Length in meters (1.0 to 2.6)
    int slatCount = 7;             // Number of slats (4 to 12)
    int frameStyle = 0;            // 0: Classic Cast Iron, 1: Modern Concrete/Steel, 2: Rustic Timber
    bool hasArmrests = true;
    bool hasBackrest = true;
    int woodTint = 0;              // 0: Warm Teak, 1: Natural Oak, 2: Dark Walnut, 3: Park Green
};

class BenchGen : public IGenerator {
public:
    BenchGen();
    ~BenchGen() override = default;

    const char* getName() const override { return "BenchGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const BenchParams& params) { params_ = params; }
    const BenchParams& getParams() const { return params_; }

private:
    BenchParams params_;
};

} // namespace procedural::props
