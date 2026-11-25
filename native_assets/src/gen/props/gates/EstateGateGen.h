#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct EstateGateParams {
    float span = 5.0f;          // Gate span width (3.0 to 9.0m)
    float pillarHeight = 2.6f;  // Pillar height (1.8 to 3.8m)
    float depth = 0.55f;        // Pillar depth
    int style = 0;              // 0: Wrought Iron Double Swing, 1: Neoclassical Stone & Bronze, 2: Modern Slat Gate, 3: Royal Gilded Palisade
    int colorScheme = 0;        // 0: Wrought Iron Black, 1: Antique Gold, 2: Heritage Green, 3: Cast Bronze
    bool hasLanterns = true;    // Pillar coach lanterns & gilded finials
};

class EstateGateGen : public IGenerator {
public:
    EstateGateGen();
    ~EstateGateGen() override = default;

    const char* getName() const override { return "EstateGateGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const EstateGateParams& params) { params_ = params; }
    const EstateGateParams& getParams() const { return params_; }

private:
    EstateGateParams params_;
};

} // namespace procedural::props
