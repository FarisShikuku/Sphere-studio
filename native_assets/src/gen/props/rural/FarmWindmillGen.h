#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct FarmWindmillParams {
    float towerHeight = 10.0f;    // Lattice tower height (6.0 to 18.0)
    float rotorDiameter = 2.4f;   // Wheel diameter (1.5 to 4.0)
    float baseWidth = 2.2f;       // Ground leg footprint width (1.2 to 4.0)
    int bladeCount = 18;          // Multi-blade wheel count (12 to 24)
    int tailStyle = 0;            // 0: Vintage Galvanized, 1: Farm Red Crest, 2: Weathered Rust
    bool hasPlatformDeck = true;  // Wooden maintenance deck platform at tower head
};

class FarmWindmillGen : public IGenerator {
public:
    FarmWindmillGen();
    ~FarmWindmillGen() override = default;

    const char* getName() const override { return "FarmWindmillGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const FarmWindmillParams& params) { params_ = params; }
    const FarmWindmillParams& getParams() const { return params_; }

private:
    FarmWindmillParams params_;
};

} // namespace procedural::props
