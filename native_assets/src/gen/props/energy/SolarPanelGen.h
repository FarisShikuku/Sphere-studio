#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include "EnergyParams.h"

namespace procedural::props {

class SolarPanelGen : public IGenerator {
public:
    SolarPanelGen();
    ~SolarPanelGen() override = default;

    const char* getName() const override { return "SolarPanelGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const SolarPanelParams& params) { params_ = params; }
    const SolarPanelParams& getParams() const { return params_; }

private:
    SolarPanelParams params_;
};

} // namespace procedural::props
