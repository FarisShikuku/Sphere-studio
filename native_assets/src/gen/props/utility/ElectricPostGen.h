#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include "UtilityParams.h"

namespace procedural::props {

class ElectricPostGen : public IGenerator {
public:
    ElectricPostGen();
    ~ElectricPostGen() override = default;

    const char* getName() const override { return "ElectricPostGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const ElectricPostParams& params) { params_ = params; }
    const ElectricPostParams& getParams() const { return params_; }

private:
    ElectricPostParams params_;
};

} // namespace procedural::props
