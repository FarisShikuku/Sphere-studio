#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include "SignageStreetParams.h"

namespace procedural::props {

class InfoBoardGen : public IGenerator {
public:
    InfoBoardGen();
    ~InfoBoardGen() override = default;

    const char* getName() const override { return "InfoBoardGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const InfoBoardParams& params) { params_ = params; }
    const InfoBoardParams& getParams() const { return params_; }

private:
    InfoBoardParams params_;
};

} // namespace procedural::props
