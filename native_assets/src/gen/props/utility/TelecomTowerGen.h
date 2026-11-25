#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include "UtilityParams.h"

namespace procedural::props {

class TelecomTowerGen : public IGenerator {
public:
    TelecomTowerGen();
    ~TelecomTowerGen() override = default;

    const char* getName() const override { return "TelecomTowerGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const TelecomTowerParams& params) { params_ = params; }
    const TelecomTowerParams& getParams() const { return params_; }

private:
    TelecomTowerParams params_;
};

} // namespace procedural::props
