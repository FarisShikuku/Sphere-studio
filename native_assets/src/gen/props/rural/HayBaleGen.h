#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct HayBaleParams {
    int baleType = 0;              // 0: Giant Round Rolled Bale, 1: Compact Rectangular Square Bale
    float radiusOrWidth = 0.75f;   // Radius for round bale (0.6 - 1.0m) or width for square
    float length = 1.35f;          // Length in meters (1.0 to 1.8m)
    int twineCount = 3;            // 2 to 4 binding twine strings
    bool wrappedPlastic = false;   // Wrapped in white/green silage wrap film
};

class HayBaleGen : public IGenerator {
public:
    HayBaleGen();
    ~HayBaleGen() override = default;

    const char* getName() const override { return "HayBaleGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const HayBaleParams& params) { params_ = params; }
    const HayBaleParams& getParams() const { return params_; }

private:
    HayBaleParams params_;
};

} // namespace procedural::props
