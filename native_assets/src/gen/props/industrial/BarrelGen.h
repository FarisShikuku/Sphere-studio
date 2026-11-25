#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct BarrelParams {
    float height = 0.90f;          // Height in meters
    float radius = 0.30f;          // Radius in meters
    int barrelType = 0;            // 0: Industrial Steel Drum, 1: Wooden Wine/Whiskey Cask, 2: Hazardous Waste
    bool hasBungPlug = true;
    int drumColor = 0;             // 0: Royal Blue, 1: Signal Red, 2: Toxic Yellow, 3: Oil Black
};

class BarrelGen : public IGenerator {
public:
    BarrelGen();
    ~BarrelGen() override = default;

    const char* getName() const override { return "BarrelGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const BarrelParams& params) { params_ = params; }
    const BarrelParams& getParams() const { return params_; }

private:
    BarrelParams params_;
};

} // namespace procedural::props
