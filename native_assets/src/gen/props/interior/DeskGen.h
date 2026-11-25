#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct DeskParams {
    float width = 1.6f;            // Width in meters (1.0 to 2.4)
    float depth = 0.8f;            // Depth (0.6 to 1.1)
    float height = 0.75f;          // Desk surface height (0.70 to 0.85)
    int drawerConfig = 1;          // 0: Minimal Table (No drawers), 1: Single Right Pedestal, 2: Executive Double Pedestal
    int finishType = 0;            // 0: Natural Oak, 1: Rich Walnut, 2: Modern White / Steel Frame
    bool hasModestyPanel = true;
    bool hasCableGrommet = true;
};

class DeskGen : public IGenerator {
public:
    DeskGen();
    ~DeskGen() override = default;

    const char* getName() const override { return "DeskGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const DeskParams& params) { params_ = params; }
    const DeskParams& getParams() const { return params_; }

private:
    DeskParams params_;
};

} // namespace procedural::props
