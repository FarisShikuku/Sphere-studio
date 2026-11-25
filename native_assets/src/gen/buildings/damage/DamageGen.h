#pragma once

#include "../../IGenerator.h"
#include "../BuildingTypes.h"
#include <vector>

namespace procedural::buildings {

enum class DamageType {
    CrackedWall,
    RubblePile,
    StructuralBreach
};

struct DamageParams {
    DamageType subType = DamageType::CrackedWall;
    Vec3 origin{0.0f, 0.0f, 0.0f};
    float severity = 0.65f; // 0.0 to 1.0
    float radius = 3.5f;
    bool exposeRebar = true;
    int32_t rubbleChunkCount = 24;
};

class DamageGen : public IGenerator {
public:
    DamageGen();
    ~DamageGen() override = default;

    const char* getName() const override { return "DamageGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Buildings; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const DamageParams& params) { params_ = params; }
    const DamageParams& getParameters() const { return params_; }

    void generateCrackedWall(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateRubblePile(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);

private:
    DamageParams params_;
};

} // namespace procedural::buildings
