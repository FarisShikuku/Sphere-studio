#include "NeonSignGen.h"
#include "../BuildingComponents.h"

namespace procedural::buildings {

NeonSignGen::NeonSignGen() = default;

bool NeonSignGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void NeonSignGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload NeonSignGen::generateMesh(const GeneratorContext& /*ctx*/) {
    std::vector<BuildingVertex> verts;
    std::vector<uint32_t> indices;

    switch (params_.subType) {
        case SignType::Billboard:
            generateBillboard(verts, indices);
            break;
        case SignType::NeonGlow:
        case SignType::FasciaStorefront:
            generateNeonSign(verts, indices);
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
