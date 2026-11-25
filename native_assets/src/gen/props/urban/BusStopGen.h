#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct BusStopParams {
    float length = 3.6f;          // Shelter length in meters (2.4 to 5.5)
    float depth = 1.6f;           // Shelter depth (1.2 to 2.2)
    float height = 2.4f;          // Roof height
    int roofType = 0;             // 0: Curved Barrel Canopy, 1: Flat Cantilever, 2: Pitched Gable
    bool hasGlassPanels = true;
    bool hasIntegratedBench = true;
    bool hasTimetableTotem = true;
    int frameColor = 0;           // 0: Matte Charcoal, 1: Stainless Steel, 2: Signal Blue
};

class BusStopGen : public IGenerator {
public:
    BusStopGen();
    ~BusStopGen() override = default;

    const char* getName() const override { return "BusStopGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const BusStopParams& params) { params_ = params; }
    const BusStopParams& getParams() const { return params_; }

private:
    BusStopParams params_;
};

} // namespace procedural::props
