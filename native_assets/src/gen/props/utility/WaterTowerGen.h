#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include "UtilityParams.h"

namespace procedural::props {

class WaterTowerGen : public IGenerator {
public:
    WaterTowerGen();
    ~WaterTowerGen() override = default;

    const char* getName() const override { return "WaterTowerGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const WaterTowerParams& params) { params_ = params; }
    const WaterTowerParams& getParams() const { return params_; }

private:
    WaterTowerParams params_;
};

} // namespace procedural::props
