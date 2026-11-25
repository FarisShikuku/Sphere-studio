#include "RoomGen.h"
#include "../BuildingComponents.h"

namespace procedural::buildings {

RoomGen::RoomGen() = default;

bool RoomGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void RoomGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload RoomGen::generateMesh(const GeneratorContext& /*ctx*/) {
    std::vector<BuildingVertex> verts;
    std::vector<uint32_t> indices;

    switch (params_.subType) {
        case RoomType::LivingRoom:
            generateLivingRoom(verts, indices);
            break;
        case RoomType::OfficeFloor:
            generateOfficeFloor(verts, indices);
            break;
        case RoomType::Floorplan:
            generateFloorplan(verts, indices);
            break;
        case RoomType::WarehouseInterior:
            generateWarehouseInterior(verts, indices);
            break;
        case RoomType::VehicleRepairInterior:
            generateVehicleRepairInterior(verts, indices);
            break;
    }

    GeneratedMeshPayload payload;
    payload.vertexStride = 9 * sizeof(float);
    payload.vertexCount = static_cast<uint32_t>(verts.size());
    payload.indexCount = static_cast<uint32_t>(indices.size());
    payload.indexBuffer = indices;

    payload.vertexBuffer.reserve(verts.size() * 9);
    for (const auto& v : verts) {
        payload.vertexBuffer.push_back(v.position.x);
        payload.vertexBuffer.push_back(v.position.y);
        payload.vertexBuffer.push_back(v.position.z);
        payload.vertexBuffer.push_back(v.normal.x);
        payload.vertexBuffer.push_back(v.normal.y);
        payload.vertexBuffer.push_back(v.normal.z);
        payload.vertexBuffer.push_back(v.uv.x);
        payload.vertexBuffer.push_back(v.uv.y);
        payload.vertexBuffer.push_back(static_cast<float>(v.materialId));
    }

    return payload;
}

} // namespace procedural::buildings
