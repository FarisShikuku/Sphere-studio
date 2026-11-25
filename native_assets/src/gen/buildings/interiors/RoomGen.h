#pragma once

#include "../../IGenerator.h"
#include "../BuildingTypes.h"
#include <vector>

namespace procedural::buildings {

enum class RoomType {
    LivingRoom = 0,
    OfficeFloor = 1,
    Floorplan = 2,
    WarehouseInterior = 3,
    VehicleRepairInterior = 4
};

struct RoomParams {
    RoomType subType = RoomType::LivingRoom;
    float width = 8.0f;
    float length = 10.0f;
    float ceilingHeight = 2.8f;
    float wallThickness = 0.15f;
    bool hasInteriorDoors = true;
    bool hasFurnitureStubs = true;
    int32_t cubicleRows = 3;
    int32_t cubicleCols = 3;
    bool showInside = true;
    int32_t interiorDesign = 0;
    int32_t loadingBays = 4;
};

class RoomGen : public IGenerator {
public:
    RoomGen();
    ~RoomGen() override = default;

    const char* getName() const override { return "RoomGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Buildings; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const RoomParams& params) { params_ = params; }
    const RoomParams& getParameters() const { return params_; }

    void generateLivingRoom(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateOfficeFloor(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateFloorplan(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateWarehouseInterior(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateVehicleRepairInterior(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);

    // Reusable interior fitout builders for enclosed building cutaway ("Show Inside")
    static void appendWarehouseInteriorContent(
        std::vector<BuildingVertex>& outVerts,
        std::vector<uint32_t>& outIndices,
        float originX, float floorY, float originZ,
        float width, float length, float height,
        int32_t designIndex, int32_t loadingBays
    );

    static void appendVehicleRepairInteriorContent(
        std::vector<BuildingVertex>& outVerts,
        std::vector<uint32_t>& outIndices,
        float originX, float floorY, float originZ,
        float width, float length, float height,
        int32_t designIndex
    );

private:
    RoomParams params_;
};

} // namespace procedural::buildings
