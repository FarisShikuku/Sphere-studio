#pragma once

#include "../../IGenerator.h"
#include "../BuildingTypes.h"
#include <vector>

namespace procedural::buildings {

enum class IndustrialType {
    Warehouse = 0,
    Factory = 1,
    Silo = 2,
    PowerPlant = 3,
    Refinery = 4,
    ContainerTerminal = 5,
    SteelMill = 6,
    CleanroomFab = 7,
    PowerLines = 8,
    WaterTreatment = 9,
    DataCenter = 10,
    AerospaceHangar = 11,
    WasteIncinerator = 12,
    BiopharmPlant = 13
};

struct IndustrialParams {
    IndustrialType subType = IndustrialType::Warehouse;
    float length = 32.0f;
    float width = 20.0f;
    float height = 8.5f;
    int32_t loadingBays = 3;
    bool hasCatwalks = true;
    bool hasSawtoothRoof = false;

    bool useExtendedParams = false;
    int32_t windowsPerFacade = 3;
    WindowType windowType = WindowType::RibbonStrip;
    float windowWidth = 1.8f;
    float windowHeight = 1.2f;
    float windowSillHeight = 1.2f;
    GlassType glassType = GlassType::ClearFloat;
    DoorType doorType = DoorType::IndustrialRollUp;
    float doorWidth = 3.2f;
    float doorHeight = 3.8f;
    bool hasEntranceCanopy = true;
    RoofType roofStyle = RoofType::Gable;
    float roofHeight = 2.4f;
    float roofOverhang = 0.6f;
    bool showRooftopClutter = true;
    WallMaterialType wallMaterial = WallMaterialType::ConcreteBrutalist;
    bool hasCornice = false;
    bool showInside = false;
    int32_t interiorDesign = 0;
};

class WarehouseGen : public IGenerator {
public:
    WarehouseGen();
    ~WarehouseGen() override = default;

    const char* getName() const override { return "WarehouseGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Buildings; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const IndustrialParams& params) { params_ = params; }
    const IndustrialParams& getParameters() const { return params_; }

    void generateWarehouse(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateFactory(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateSilo(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generatePowerPlant(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateRefinery(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateContainerTerminal(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateSteelMill(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCleanroomFab(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generatePowerLines(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateWaterTreatment(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateDataCenter(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateAerospaceHangar(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateWasteIncinerator(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateBiopharmPlant(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);

private:
    IndustrialParams params_;
};

} // namespace procedural::buildings
