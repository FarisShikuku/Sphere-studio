#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include "EnergyParams.h"

namespace procedural::props {

class SubstationGen : public IGenerator {
public:
    SubstationGen();
    ~SubstationGen() override = default;

    const char* getName() const override { return "SubstationGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const SubstationParams& params) { params_ = params; }
    const SubstationParams& getParams() const { return params_; }

private:
    SubstationParams params_;
};

} // namespace procedural::props
