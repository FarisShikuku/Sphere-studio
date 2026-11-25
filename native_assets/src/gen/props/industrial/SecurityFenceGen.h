#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct SecurityFenceParams {
    float length = 5.0f;        // Length in meters (2.0 to 12.0)
    float height = 2.8f;        // Fence height (1.8 to 4.5)
    float postSpacing = 2.2f;   // Post spacing (1.5 to 3.2)
    int fenceStyle = 0;         // 0: 358 Anti-Climb Mesh, 1: Palisade Trident, 2: High-Voltage Electric, 3: Crash Cable
    int colorScheme = 0;        // 0: Galvanized Zinc, 1: Security Black, 2: Military Olive, 3: Safety Yellow
    bool hasRazorWire = true;   // Overhead concertina razor wire coils
};

class SecurityFenceGen : public IGenerator {
public:
    SecurityFenceGen();
    ~SecurityFenceGen() override = default;

    const char* getName() const override { return "SecurityFenceGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const SecurityFenceParams& params) { params_ = params; }
    const SecurityFenceParams& getParams() const { return params_; }

private:
    SecurityFenceParams params_;
};

} // namespace procedural::props
