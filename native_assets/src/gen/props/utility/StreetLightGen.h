#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include "UtilityParams.h"

namespace procedural::props {

class StreetLightGen : public IGenerator {
public:
    StreetLightGen();
    ~StreetLightGen() override = default;

    const char* getName() const override { return "StreetLightGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const StreetLightParams& params) { params_ = params; }
    const StreetLightParams& getParams() const { return params_; }

private:
    StreetLightParams params_;
};

} // namespace procedural::props
