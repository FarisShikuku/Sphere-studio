#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct TollGateParams {
    float laneWidth = 3.8f;      // Lane width in meters (3.0 to 6.0m)
    float kioskHeight = 3.0f;    // Kiosk/canopy height (2.4 to 4.5m)
    float barrierOffset = 0.5f;
    int style = 0;               // 0: Highway Toll Plaza, 1: Automated Smart Toll ETC, 2: Parking Access Totem
    int colorScheme = 0;         // 0: Standard, 1: Orange Safety, 2: Toll Blue
    bool hasCanopy = true;       // Overhead canopy & LED lane signals
};

class TollGateGen : public IGenerator {
public:
    TollGateGen();
    ~TollGateGen() override = default;

    const char* getName() const override { return "TollGateGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const TollGateParams& params) { params_ = params; }
    const TollGateParams& getParams() const { return params_; }

private:
    TollGateParams params_;
};

} // namespace procedural::props
