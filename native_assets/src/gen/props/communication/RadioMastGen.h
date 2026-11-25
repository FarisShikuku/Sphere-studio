#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include "CommunicationParams.h"

namespace procedural::props {

class RadioMastGen : public IGenerator {
public:
    RadioMastGen();
    ~RadioMastGen() override = default;

    const char* getName() const override { return "RadioMastGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const RadioMastParams& params) { params_ = params; }
    const RadioMastParams& getParams() const { return params_; }

private:
    RadioMastParams params_;
};

} // namespace procedural::props
