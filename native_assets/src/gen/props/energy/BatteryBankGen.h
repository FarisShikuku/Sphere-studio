#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include "EnergyParams.h"

namespace procedural::props {

class BatteryBankGen : public IGenerator {
public:
    BatteryBankGen();
    ~BatteryBankGen() override = default;

    const char* getName() const override { return "BatteryBankGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const BatteryBankParams& params) { params_ = params; }
    const BatteryBankParams& getParams() const { return params_; }

private:
    BatteryBankParams params_;
};

} // namespace procedural::props
