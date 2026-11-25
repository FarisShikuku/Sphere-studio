#pragma once

#include "../../IGenerator.h"
#include "../BuildingTypes.h"
#include "TowerGen.h"
#include <vector>

namespace procedural::buildings {

class CarWashGen : public IGenerator {
public:
    CarWashGen();
    ~CarWashGen() override = default;

    const char* getName() const override { return "CarWashGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Buildings; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const CommercialParams& params) { params_ = params; }
    const CommercialParams& getParameters() const { return params_; }

    void generate(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);

private:
    CommercialParams params_;
};

} // namespace procedural::buildings
