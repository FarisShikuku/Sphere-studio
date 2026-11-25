#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct WaterTankParams {
    float tankHeight = 2.4f;           // Tank body height or length (1.0 to 5.5 m)
    float tankDiameter = 1.8f;         // Tank cylinder diameter (0.8 to 4.0 m)
    float standHeight = 2.0f;          // Elevation height for elevated steel stand (1.0 to 5.0 m)
    int tankStyle = 0;                 // 0: Ribbed Poly TopTank, 1: Elevated Steel-Stand Tank, 2: Horizontal Saddle Cistern, 3: Corrugated Farm Tank
    int colorScheme = 0;               // 0: TopTank Black, 1: Forest Green, 2: Ocean Blue, 3: Desert Tan, 4: Galvanized Steel
    bool hasPlumbingAndLadder = true;  // Brass valves, inlet/outlet pipes, sight gauge & inspection ladder
};

class WaterTankGen : public IGenerator {
public:
    WaterTankGen();
    ~WaterTankGen() override = default;

    const char* getName() const override { return "WaterTankGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const WaterTankParams& params) { params_ = params; }
    const WaterTankParams& getParams() const { return params_; }

private:
    WaterTankParams params_;
};

} // namespace procedural::props
