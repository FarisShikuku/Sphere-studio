#pragma once

#include "../../IGenerator.h"
#include "../BuildingTypes.h"
#include <vector>

namespace procedural::buildings {

enum class ImpostorType {
    CityBlockOctahedral,
    BillboardQuad,
    FacadeCard
};

struct ImpostorParams {
    ImpostorType subType = ImpostorType::CityBlockOctahedral;
    float blockWidth = 60.0f;
    float blockDepth = 60.0f;
    int32_t buildingCountX = 3;
    int32_t buildingCountZ = 3;
    float maxBuildingHeight = 28.0f;
    int32_t textureAtlasResolution = 1024;
};

class ImpostorGen : public IGenerator {
public:
    ImpostorGen();
    ~ImpostorGen() override = default;

    const char* getName() const override { return "ImpostorGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Buildings; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const ImpostorParams& params) { params_ = params; }
    const ImpostorParams& getParameters() const { return params_; }

    void generateCityBlock(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateBillboardCards(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);

private:
    ImpostorParams params_;
};

} // namespace procedural::buildings
