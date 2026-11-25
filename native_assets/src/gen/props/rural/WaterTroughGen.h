#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct WaterTroughParams {
    float length = 1.8f;           // Length in meters (1.2 to 2.5)
    float width = 0.65f;           // Width (0.5 to 0.9)
    float height = 0.55f;          // Basin height (0.4 to 0.7)
    int troughMaterial = 0;        // 0: Galvanized Corrugated Steel, 1: Carved Stone / Masonry, 2: Timber Planks
    float waterLevel = 0.85f;      // Water fullness (0.0 to 1.0)
    bool hasFloatValve = true;     // Brass float ball and supply pipe
};

class WaterTroughGen : public IGenerator {
public:
    WaterTroughGen();
    ~WaterTroughGen() override = default;

    const char* getName() const override { return "WaterTroughGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const WaterTroughParams& params) { params_ = params; }
    const WaterTroughParams& getParams() const { return params_; }

private:
    WaterTroughParams params_;
};

} // namespace procedural::props
