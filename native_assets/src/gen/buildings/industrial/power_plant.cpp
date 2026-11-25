#include "WarehouseGen.h"
#include "../BuildingComponents.h"
#include <cmath>
#include <algorithm>

namespace procedural::buildings {

void WarehouseGen::generatePowerPlant(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(48.0f, std::max(params_.width, params_.length));
    float d = std::max(32.0f, std::min(params_.width, params_.length));
    float h = std::max(8.0f, params_.height);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallConcrete = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t tarmacMat = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t silverAlum = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t darkMetal = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t safetyYellow = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t hvacMat = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t doorMetal = static_cast<uint32_t>(MaterialSlot::DoorWoodMetal);

    // 1. Industrial Concrete Foundation & Substation Pad
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 6.0f, -0.4f, -hd - 6.0f},
        Vec3{ hw + 6.0f,  0.0f,  hd + 6.0f},
        tarmacMat);

    // 2. Massive Hyperbolic Cooling Tower (Left Section)
    float towerCenterX = -hw + 14.0f;
    float towerCenterZ = 0.0f;
    float towerH = std::max(22.0f, h * 1.8f);
    float rBase = 11.5f;
    float rThroat = 8.0f;
    float rTop = 9.5f;
    float yThroat = towerH * 0.72f;

    // Stacked hyperbolic rings
    const int heightSteps = 10;
    const int radialSegments = 16;
    for (int yStep = 0; yStep < heightSteps; ++yStep) {
        float y0 = (static_cast<float>(yStep) / heightSteps) * towerH;
        float y1 = (static_cast<float>(yStep + 1) / heightSteps) * towerH;

        // Hyperbolic profile interpolation
        float t0 = (y0 - yThroat) / (towerH * 0.5f);
        float t1 = (y1 - yThroat) / (towerH * 0.5f);
        float r0 = rThroat + (rBase - rThroat) * (1.0f - y0 / towerH) * (1.0f - y0 / towerH) + (rTop - rThroat) * (y0 > yThroat ? ((y0 - yThroat) / (towerH - yThroat)) : 0.0f);
        float r1 = rThroat + (rBase - rThroat) * (1.0f - y1 / towerH) * (1.0f - y1 / towerH) + (rTop - rThroat) * (y1 > yThroat ? ((y1 - yThroat) / (towerH - yThroat)) : 0.0f);

        for (int i = 0; i < radialSegments; ++i) {
            float ang0 = (static_cast<float>(i) / radialSegments) * 6.28318f;
            float ang1 = (static_cast<float>(i + 1) / radialSegments) * 6.28318f;

            float x00 = towerCenterX + r0 * std::cos(ang0);
            float z00 = towerCenterZ + r0 * std::sin(ang0);
            float x01 = towerCenterX + r0 * std::cos(ang1);
            float z01 = towerCenterZ + r0 * std::sin(ang1);

            float x10 = towerCenterX + r1 * std::cos(ang0);
            float z10 = towerCenterZ + r1 * std::sin(ang0);
            float x11 = towerCenterX + r1 * std::cos(ang1);
            float z11 = towerCenterZ + r1 * std::sin(ang1);

            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{std::min({x00, x01, x10, x11}), y0, std::min({z00, z01, z10, z11})},
                Vec3{std::max({x00, x01, x10, x11}), y1, std::max({z00, z01, z10, z11})},
                wallConcrete);
        }
    }

    // Cooling Tower Angled Concrete Support Legs at Base
    for (int i = 0; i < 8; ++i) {
        float ang = (static_cast<float>(i) / 8.0f) * 6.28318f;
        float legX = towerCenterX + (rBase + 0.3f) * std::cos(ang);
        float legZ = towerCenterZ + (rBase + 0.3f) * std::sin(ang);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{legX - 0.45f, 0.0f, legZ - 0.45f},
            Vec3{legX + 0.45f, 2.8f, legZ + 0.45f},
            foundMat);
    }
    // Cooling Tower Top Rim Flange Ring
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{towerCenterX - rTop - 0.6f, towerH, towerCenterZ - rTop - 0.6f},
        Vec3{towerCenterX + rTop + 0.6f, towerH + 0.6f, towerCenterZ + rTop + 0.6f},
        silverAlum);

    // 3. Heavy Turbine & Generator Hall Building (Right Section)
    float hallW = 24.0f;
    float hallD = 20.0f;
    float hallH = h;
    float hallX = hw - hallW * 0.5f - 2.0f;
    float hallZ = 0.0f;

    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hallX - hallW * 0.5f, 0.0f, hallZ - hallD * 0.5f},
        Vec3{hallX + hallW * 0.5f, hallH, hallZ + hallD * 0.5f},
        wallConcrete);

    // Hall Pitched Gable Monitor Roof
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hallX - hallW * 0.5f - 0.4f, hallH, hallZ - hallD * 0.5f - 0.4f},
        Vec3{hallX + hallW * 0.5f + 0.4f, hallH + 2.6f, hallZ + hallD * 0.5f + 0.4f},
        darkMetal);

    // Clerestory Ribbon Windows along upper hall walls
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hallX - hallW * 0.5f + 2.0f, hallH - 2.2f, hallZ + hallD * 0.5f - 0.1f},
        Vec3{hallX + hallW * 0.5f - 2.0f, hallH - 0.6f, hallZ + hallD * 0.5f + 0.15f},
        glassMat);

    // Steel Structural Portal Frames along Hall Exterior
    for (float px = hallX - hallW * 0.5f + 3.0f; px < hallX + hallW * 0.5f; px += 4.5f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 0.3f, 0.0f, hallZ + hallD * 0.5f},
            Vec3{px + 0.3f, hallH, hallZ + hallD * 0.5f + 0.45f},
            frameMat);
    }

    // Heavy Industrial Roll-Up Loading Doors for Generator Rotor Service
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hallX - 3.2f, 0.0f, hallZ + hallD * 0.5f + 0.05f},
        Vec3{hallX + 3.2f, 5.2f, hallZ + hallD * 0.5f + 0.25f},
        doorMetal);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hallX - 3.4f, 5.2f, hallZ + hallD * 0.5f},
        Vec3{hallX + 3.4f, 5.5f, hallZ + hallD * 0.5f + 0.8f},
        safetyYellow);

    // 4. Elevated Steam Pipeline Gantry connecting Turbine Hall & Tower
    float gantryY = hallH * 0.65f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{towerCenterX + rBase * 0.5f, gantryY, -1.2f},
        Vec3{hallX - hallW * 0.5f, gantryY + 1.2f, 1.2f},
        silverAlum);
    // Vertical Structural Support Trestles for pipe gantry
    float midX = (towerCenterX + rBase * 0.5f + hallX - hallW * 0.5f) * 0.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{midX - 0.35f, 0.0f, -0.35f},
        Vec3{midX + 0.35f, gantryY, 0.35f},
        frameMat);

    // 5. Electrical Transformer Substation Yard (Rear of Hall)
    float subX = hallX;
    float subZ = -hd - 2.0f;
    // Blast Barrier Concrete Wall around transformers
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{subX - 8.0f, 0.0f, subZ - 3.5f},
        Vec3{subX + 8.0f, 3.2f, subZ - 3.0f},
        wallConcrete);

    // 3 Step-Up High Voltage Transformers with Fin Radiators
    for (float tx : {subX - 5.0f, subX, subX + 5.0f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{tx - 1.6f, 0.0f, subZ - 2.2f},
            Vec3{tx + 1.6f, 2.6f, subZ + 0.8f},
            hvacMat);
        // High voltage ceramic insulator bushings on transformer top
        for (float bx : {tx - 0.8f, tx, tx + 0.8f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{bx - 0.15f, 2.6f, subZ - 0.7f},
                Vec3{bx + 0.15f, 4.0f, subZ - 0.4f},
                silverAlum);
        }
    }

    // 6. High-Voltage Steel Lattice Transmission Pylon Mast
    float mastX = hw + 3.0f;
    float mastZ = -hd - 3.0f;
    float mastH = 24.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{mastX - 0.8f, 0.0f, mastZ - 0.8f},
        Vec3{mastX + 0.8f, mastH, mastZ + 0.8f},
        frameMat);
    // Pylon Cross-Arms
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{mastX - 4.2f, mastH - 3.0f, mastZ - 0.3f},
        Vec3{mastX + 4.2f, mastH - 2.5f, mastZ + 0.3f},
        frameMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{mastX - 3.2f, mastH - 6.0f, mastZ - 0.3f},
        Vec3{mastX + 3.2f, mastH - 5.5f, mastZ + 0.3f},
        frameMat);
}

} // namespace procedural::buildings
