#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include "EnergyParams.h"
#include "SolarTracker.h"

namespace procedural::props {

class SolarFarmGen : public IGenerator {
public:
    SolarFarmGen();
    ~SolarFarmGen() override = default;

    const char* getName() const override { return "SolarFarmGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const SolarFarmParams& params) { params_ = params; }
    const SolarFarmParams& getParams() const { return params_; }

private:
    SolarFarmParams params_;
};

} // namespace procedural::props
