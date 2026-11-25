#include "ImpostorGen.h"
#include "../BuildingComponents.h"
#include <cmath>

namespace procedural::buildings {

ImpostorGen::ImpostorGen() = default;

bool ImpostorGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void ImpostorGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload ImpostorGen::generateMesh(const GeneratorContext& /*ctx*/) {
    std::vector<BuildingVertex> verts;
    std::vector<uint32_t> indices;

    switch (params_.subType) {
        case ImpostorType::CityBlockOctahedral:
            generateCityBlock(verts, indices);
            break;
        case ImpostorType::BillboardQuad:
        case ImpostorType::FacadeCard:
            generateBillboardCards(verts, indices);
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

void ImpostorGen::generateCityBlock(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float bw = params_.blockWidth;
    float bd = params_.blockDepth;
    int nx = params_.buildingCountX;
    int nz = params_.buildingCountZ;

    uint32_t roadMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t wallMat = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);

    float hbw = bw * 0.5f;
    float hbd = bd * 0.5f;

    // Asphalt street ground base plane
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hbw - 4.0f, -0.4f, -hbd - 4.0f}, Vec3{hbw + 4.0f, 0.0f, hbd + 4.0f}, roadMat);

    // Concrete city sidewalk plinth
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hbw, 0.0f, -hbd}, Vec3{hbw, 0.2f, hbd}, roadMat);

    float lotW = (bw - 4.0f) / nx;
    float lotD = (bd - 4.0f) / nz;

    for (int i = 0; i < nx; ++i) {
        float x0 = -hbw + 2.0f + i * lotW + 0.8f;
        float x1 = x0 + lotW - 1.6f;

        for (int j = 0; j < nz; ++j) {
            float z0 = -hbd + 2.0f + j * lotD + 0.8f;
            float z1 = z0 + lotD - 1.6f;

            // Pseudo-random deterministic height
            float seed = std::sin(static_cast<float>(i * 17 + j * 31));
            float bH = 10.0f + std::abs(seed) * (params_.maxBuildingHeight - 10.0f);
            uint32_t mat = (i + j) % 2 == 0 ? wallMat : glassMat;

            // Simplified building box massing
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{x0, 0.2f, z0},
                Vec3{x1, bH, z1},
                mat
            );

            // Roof cap
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{x0 + 0.2f, bH, z0 + 0.2f},
                Vec3{x1 - 0.2f, bH + 0.6f, z1 - 0.2f},
                roofMat
            );
        }
    }
}

void ImpostorGen::generateBillboardCards(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float bw = params_.blockWidth;
    float h = params_.maxBuildingHeight;
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);

    // Cross-billboard impostor cards (2 intersecting quads in X and Z)
    float hw = bw * 0.5f;

    // Card 1: along X axis
    Vec3 nZ{0.0f, 0.0f, 1.0f};
    BuildingComponents::appendQuad(
        outVerts, outIndices,
        Vec3{-hw, 0.0f, 0.0f}, Vec3{hw, 0.0f, 0.0f},
        Vec3{hw, h, 0.0f}, Vec3{-hw, h, 0.0f},
        nZ, glassMat
    );

    // Card 2: along Z axis
    Vec3 nX{1.0f, 0.0f, 0.0f};
    BuildingComponents::appendQuad(
        outVerts, outIndices,
        Vec3{0.0f, 0.0f, -hw}, Vec3{0.0f, 0.0f, hw},
        Vec3{0.0f, h, hw}, Vec3{0.0f, h, -hw},
        nX, glassMat
    );
}

} // namespace procedural::buildings
