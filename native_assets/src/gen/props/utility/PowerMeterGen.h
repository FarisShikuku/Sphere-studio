#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include "UtilityParams.h"

namespace procedural::props {

class PowerMeterGen : public IGenerator {
public:
    PowerMeterGen();
    ~PowerMeterGen() override = default;

    const char* getName() const override { return "PowerMeterGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const PowerMeterParams& params) { params_ = params; }
    const PowerMeterParams& getParams() const { return params_; }

private:
    PowerMeterParams params_;
};

} // namespace procedural::props
