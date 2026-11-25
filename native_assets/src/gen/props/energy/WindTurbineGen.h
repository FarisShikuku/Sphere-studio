#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include "EnergyParams.h"
#include "WindTurbineTower.h"
#include "WindTurbineBlades.h"

namespace procedural::props {

class WindTurbineGen : public IGenerator {
public:
    WindTurbineGen();
    ~WindTurbineGen() override = default;

    const char* getName() const override { return "WindTurbineGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const WindTurbineParams& params) { params_ = params; }
    const WindTurbineParams& getParams() const { return params_; }

private:
    WindTurbineParams params_;
};

} // namespace procedural::props
