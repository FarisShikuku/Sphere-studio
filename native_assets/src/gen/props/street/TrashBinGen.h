#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct TrashBinParams {
    float height = 0.95f;          // Bin height
    float radius = 0.28f;          // Radius or half-width
    int binType = 0;               // 0: Cylindrical Metal Mesh, 1: Square Wooden Slatted, 2: Dual Recycling Station
    bool hasRainCover = true;
    bool hasPostMount = false;     // Ground standing vs side post mounted
    int bodyColor = 0;             // 0: Forest Green, 1: Anthracite Grey, 2: Cobalt Blue
};

class TrashBinGen : public IGenerator {
public:
    TrashBinGen();
    ~TrashBinGen() override = default;

    const char* getName() const override { return "TrashBinGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const TrashBinParams& params) { params_ = params; }
    const TrashBinParams& getParams() const { return params_; }

private:
    TrashBinParams params_;
};

} // namespace procedural::props
