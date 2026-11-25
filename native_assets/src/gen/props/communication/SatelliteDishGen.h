#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include "CommunicationParams.h"

namespace procedural::props {

class SatelliteDishGen : public IGenerator {
public:
    SatelliteDishGen();
    ~SatelliteDishGen() override = default;

    const char* getName() const override { return "SatelliteDishGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const SatelliteDishParams& params) { params_ = params; }
    const SatelliteDishParams& getParams() const { return params_; }

private:
    SatelliteDishParams params_;
};

} // namespace procedural::props
