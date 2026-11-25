#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct ExpandableBarricadeParams {
    float span = 2.5f;          // Span in meters (1.5 to 5.0)
    float height = 1.1f;        // Height (0.8 to 1.6)
    float footWidth = 0.50f;    // Foot depth (0.35 to 0.8)
    int barricadeStyle = 0;     // 0: Steel Crowd Bike Rack, 1: Scissor Accordion, 2: Water-Filled Plastic, 3: A-Frame Traffic
    int colorScheme = 0;        // 0: Safety Orange, 1: Traffic Yellow, 2: Galvanized Silver, 3: Traffic Red
    bool hasFlashers = true;    // Reflective hazard striping, amber strobe flashers
};

class ExpandableBarricadeGen : public IGenerator {
public:
    ExpandableBarricadeGen();
    ~ExpandableBarricadeGen() override = default;

    const char* getName() const override { return "ExpandableBarricadeGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const ExpandableBarricadeParams& params) { params_ = params; }
    const ExpandableBarricadeParams& getParams() const { return params_; }

private:
    ExpandableBarricadeParams params_;
};

} // namespace procedural::props
