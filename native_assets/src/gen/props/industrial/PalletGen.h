#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct PalletParams {
    float length = 1.20f;         // Length (1200mm Euro/GMA standard)
    float width = 0.80f;          // Width (800mm Euro standard or 1000mm GMA)
    float height = 0.144f;        // 144mm standard pallet height
    int topBoardCount = 5;        // 5 to 9 boards
    int palletSpec = 0;           // 0: Euro Pallet (EUR-1), 1: American GMA (1200x1000), 2: Heavy Industrial
    bool hasChamfers = true;
};

class PalletGen : public IGenerator {
public:
    PalletGen();
    ~PalletGen() override = default;

    const char* getName() const override { return "PalletGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const PalletParams& params) { params_ = params; }
    const PalletParams& getParams() const { return params_; }

private:
    PalletParams params_;
};

} // namespace procedural::props
