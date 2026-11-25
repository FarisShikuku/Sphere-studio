#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"
#include "SignageStreetParams.h"

namespace procedural::props {

class KilometreMarkerGen : public IGenerator {
public:
    KilometreMarkerGen();
    ~KilometreMarkerGen() override = default;

    const char* getName() const override { return "KilometreMarkerGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const KilometreMarkerParams& params) { params_ = params; }
    const KilometreMarkerParams& getParams() const { return params_; }

private:
    KilometreMarkerParams params_;
};

} // namespace procedural::props
