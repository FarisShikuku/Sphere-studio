#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include "UtilityParams.h"

namespace procedural::props {

class TransformerGen : public IGenerator {
public:
    TransformerGen();
    ~TransformerGen() override = default;

    const char* getName() const override { return "TransformerGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const TransformerParams& params) { params_ = params; }
    const TransformerParams& getParams() const { return params_; }

private:
    TransformerParams params_;
};

} // namespace procedural::props
