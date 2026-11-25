#include "PetrolStationGen.h"

namespace procedural::buildings {

PetrolStationGen::PetrolStationGen() {
    params_.subType = CommercialType::PetrolStation;
    params_.footprintWidth = 24.0f;
    params_.footprintDepth = 16.0f;
    params_.storyHeight = 4.8f;
    params_.stories = 1;
}

bool PetrolStationGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void PetrolStationGen::update(float /*timeSeconds*/, float /*dt*/) {}

void PetrolStationGen::generate(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices) {
    TowerGen towerGen;
    CommercialParams p = params_;
    p.subType = CommercialType::PetrolStation;
    towerGen.configure(p);
    towerGen.generatePetrolStation(outVerts, outIndices);
}

GeneratedMeshPayload PetrolStationGen::generateMesh(const GeneratorContext& /*ctx*/) {
    std::vector<BuildingVertex> verts;
    std::vector<uint32_t> indices;

    generate(verts, indices);

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
