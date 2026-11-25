#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct ChairParams {
    int chairType = 0;             // 0: Ergonomic Mesh Office Swivel, 1: Classic Dining Chair, 2: Club Armchair
    float seatHeight = 0.46f;      // Seat height (0.40 to 0.55m)
    bool hasArmrests = true;
    bool hasWheels = true;         // 5-star swivel caster base
    int upholsteryColor = 0;       // 0: Stealth Black Mesh, 1: Slate Grey, 2: Cognac Leather, 3: Ocean Teal
};

class ChairGen : public IGenerator {
public:
    ChairGen();
    ~ChairGen() override = default;

    const char* getName() const override { return "ChairGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const ChairParams& params) { params_ = params; }
    const ChairParams& getParams() const { return params_; }

private:
    ChairParams params_;
};

} // namespace procedural::props
