#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct SecurityGateParams {
    float width = 5.0f;         // Width in meters (2.5 to 10.0)
    float height = 2.2f;        // Height (1.0 to 3.5)
    float trackOffset = 0.10f;  // Track clearance (0.05 to 0.3)
    int gateStyle = 0;          // 0: Cantilever Sliding, 1: Double-Swing, 2: Traffic Boom Barrier, 3: Full-Height Turnstile
    int colorScheme = 0;        // 0: Galvanized Steel, 1: Jet Black, 2: Safety Yellow, 3: Traffic White
    bool isMotorized = true;    // Motorized operator cabinet, access keypad pedestal, strobe warning beacon
};

class SecurityGateGen : public IGenerator {
public:
    SecurityGateGen();
    ~SecurityGateGen() override = default;

    const char* getName() const override { return "SecurityGateGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const SecurityGateParams& params) { params_ = params; }
    const SecurityGateParams& getParams() const { return params_; }

private:
    SecurityGateParams params_;
};

} // namespace procedural::props
