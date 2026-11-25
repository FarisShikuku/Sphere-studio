#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct PipeParams {
    float length = 2.4f;           // Length in meters
    float radius = 0.15f;          // Outer pipe radius
    int pipeConfig = 0;            // 0: Straight Flanged Spool, 1: 90-Degree Elbow Spool, 2: T-Junction with Valve
    bool hasHandwheelValve = true; // Gate valve with circular handwheel
    int flangeCount = 2;           // 2 to 4 flanges with bolts
    int materialType = 0;          // 0: Industrial Steel / Galvanized, 1: High-Vis Yellow Gas, 2: Steam Insulated
};

class PipeGen : public IGenerator {
public:
    PipeGen();
    ~PipeGen() override = default;

    const char* getName() const override { return "PipeGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const PipeParams& params) { params_ = params; }
    const PipeParams& getParams() const { return params_; }

private:
    PipeParams params_;
};

} // namespace procedural::props
