#include "CarWashGen.h"

namespace procedural::buildings {

CarWashGen::CarWashGen() {
    params_.subType = CommercialType::CarWash;
    params_.footprintWidth = 20.0f;
    params_.footprintDepth = 22.0f;
    params_.storyHeight = 4.2f;
    params_.stories = 1;
}

bool CarWashGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void CarWashGen::update(float /*timeSeconds*/, float /*dt*/) {}

void CarWashGen::generate(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices) {
    TowerGen towerGen;
    CommercialParams p = params_;
    p.subType = CommercialType::CarWash;
    towerGen.configure(p);
    towerGen.generateCarWash(outVerts, outIndices);
}

GeneratedMeshPayload CarWashGen::generateMesh(const GeneratorContext& /*ctx*/) {
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
