#include "CivicGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>

namespace procedural::buildings {

void CivicGen::generateParishChapel(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(14.0f, params_.mainWidth * 0.65f);
    float d = std::max(22.0f, params_.mainDepth * 1.15f);
    float hw = w * 0.5f;
    float hd = d * 0.5f;
    float wallH = 6.4f;

    uint32_t foundMat   = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t stuccoMat  = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t brickMat   = static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t corniceMat = static_cast<uint32_t>(MaterialSlot::CorniceStone);
    uint32_t roofMat    = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t brassMat   = static_cast<uint32_t>(MaterialSlot::BrassHardware);
    uint32_t glassMat   = static_cast<uint32_t>(MaterialSlot::Glass);

    // 1. Stone Entry Terrace & Steps (+Z Front)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, -0.35f, -hd - 1.5f},
        Vec3{ hw + 2.0f,  0.00f,  hd + 4.5f},
        foundMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.3f, 0.00f, -hd - 0.3f},
        Vec3{ hw + 0.3f, 0.65f,  hd + 2.6f},
        brickMat);
    for (int s = 0; s < 4; ++s) {
        float sy = (s + 1) * (0.65f / 4.0f);
        float sz = hd + 4.0f - s * 0.35f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-3.8f, 0.0f, sz - 0.36f},
            Vec3{ 3.8f, sy,   sz},
            corniceMat);
    }

    // 2. Classic Crisp White Chapel Sanctuary Hall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.65f, -hd},
        Vec3{ hw, wallH,  hd - 1.5f},
        stuccoMat);
    // Classical cornice eaves band
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.25f, wallH - 0.35f, -hd - 0.25f},
        Vec3{ hw + 0.25f, wallH + 0.15f,  hd - 1.25f},
        corniceMat);

    // Steep Pitched Roof Running Front-to-Back along Z
    float ridgeH = 4.2f;
    float rZ0 = -hd - 0.4f;
    float rZ1 =  hd - 1.1f;
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{-hw - 0.5f, wallH, rZ1},
        Vec3{-hw - 0.5f, wallH, rZ0},
        Vec3{0.0f, wallH + ridgeH, rZ0},
        Vec3{0.0f, wallH + ridgeH, rZ1},
        Vec3{-0.70f, 0.71f, 0.0f}, roofMat);
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{ hw + 0.5f, wallH, rZ0},
        Vec3{ hw + 0.5f, wallH, rZ1},
        Vec3{0.0f, wallH + ridgeH, rZ1},
        Vec3{0.0f, wallH + ridgeH, rZ0},
        Vec3{ 0.70f, 0.71f, 0.0f}, roofMat);
    // Gable front & rear triangular infill walls
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{-hw, wallH, hd - 1.5f},
        Vec3{ hw, wallH, hd - 1.5f},
        Vec3{0.1f, wallH + ridgeH, hd - 1.5f},
        Vec3{-0.1f, wallH + ridgeH, hd - 1.5f},
        Vec3{0, 0, 1}, stuccoMat);
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{ hw, wallH, -hd},
        Vec3{-hw, wallH, -hd},
        Vec3{-0.1f, wallH + ridgeH, -hd},
        Vec3{0.1f, wallH + ridgeH, -hd},
        Vec3{0, 0, -1}, stuccoMat);

    // 3. Tall Arched Stained & Clear Nave Windows along Left (-X) & Right (+X) Flanks
    for (int side : {-1, 1}) {
        for (float wz = -hd + 3.0f; wz <= hd - 4.0f; wz += 3.8f) {
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{side * hw, 3.65f, wz},
                1.5f, 3.2f,
                Vec3{static_cast<float>(side), 0.0f, 0.0f}, Vec3{0, 0, 1},
                WindowType::ArchedHistoric, GlassType::StainedGlassArtisan);
        }
    }

    // 4. Classical Columnar Entrance Portico (+Z Front)
    float portZ0 = hd - 1.5f;
    float portZ1 = hd + 2.0f;
    float portHW = std::min(4.6f, hw * 0.65f);
    for (float cx : {-portHW + 0.4f, -portHW * 0.35f, portHW * 0.35f, portHW - 0.4f}) {
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{cx, 0.65f, portZ1 - 0.4f},
            Vec3{cx, wallH - 0.3f, portZ1 - 0.4f},
            0.30f, 0.26f, 12, stuccoMat);
    }
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-portHW, wallH - 0.3f, portZ0},
        Vec3{ portHW, wallH + 0.3f, portZ1},
        corniceMat);
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{0.0f, 0.65f, portZ0},
        2.4f, 3.4f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::ArchedWooden, false);

    // 5. Soaring Square Belfry Tower + Octagonal Louvered Cupola & Slender White Spire
    float stX = 0.0f;
    float stZ = hd - 3.6f;
    float baseBoxHW = 2.1f;
    float belfryBaseY = wallH + ridgeH - 1.2f;
    float belfryTopY  = belfryBaseY + 4.4f;

    // Square Clock Tower Drum rising from front ridge
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-baseBoxHW, wallH, stZ - baseBoxHW},
        Vec3{ baseBoxHW, belfryTopY, stZ + baseBoxHW},
        stuccoMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-baseBoxHW - 0.25f, belfryTopY, stZ - baseBoxHW - 0.25f},
        Vec3{ baseBoxHW + 0.25f, belfryTopY + 0.35f, stZ + baseBoxHW + 0.25f},
        corniceMat);
    // Illuminated circular church clock face on front (+Z) of drum
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{stX, belfryTopY - 1.4f, stZ + baseBoxHW},
        Vec3{stX, belfryTopY - 1.4f, stZ + baseBoxHW + 0.14f},
        0.95f, 0.95f, 16, glassMat);

    // Octagonal Louvered Bell Lantern Stage
    float lanTop = belfryTopY + 3.4f;
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{stX, belfryTopY + 0.35f, stZ},
        Vec3{stX, lanTop, stZ},
        1.45f, 1.45f, 8, stuccoMat);
    // Gold Bell Hanging Inside / Accent Band
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{stX, belfryTopY + 1.2f, stZ},
        Vec3{stX, belfryTopY + 2.4f, stZ},
        1.50f, 1.50f, 8, corniceMat);

    // Slender White Octagonal Needle Spire + Brass Cross
    float needleTop = lanTop + 8.5f;
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{stX, lanTop, stZ},
        Vec3{stX, needleTop, stZ},
        1.40f, 0.06f, 8, stuccoMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-0.08f, needleTop, stZ - 0.08f},
        Vec3{ 0.08f, needleTop + 1.8f, stZ + 0.08f},
        brassMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-0.55f, needleTop + 1.15f, stZ - 0.07f},
        Vec3{ 0.55f, needleTop + 1.30f, stZ + 0.07f},
        brassMat);
}

} // namespace procedural::buildings
