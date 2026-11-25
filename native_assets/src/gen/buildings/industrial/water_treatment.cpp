#include "WarehouseGen.h"
#include "../BuildingComponents.h"
#include <cmath>

namespace procedural::buildings {

void WarehouseGen::generateWaterTreatment(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float l = std::max(36.0f, params_.length);
    float w = std::max(24.0f, params_.width);
    float hl = l * 0.5f;
    float hw = w * 0.5f;

    uint32_t concMat  = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t waterMat = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t steelMat = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t pipeMat  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t safetyYellow = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t tarmacMat = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t glassMat  = static_cast<uint32_t>(MaterialSlot::Glass);

    // 1. Concrete ground apron and foundation slab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.5f, -0.4f, -hl - 1.5f},
        Vec3{hw + 1.5f, 0.0f, hl + 1.5f},
        tarmacMat);

    // 2. Primary Pump & Membrane Filtration Building (front left: -hw to 0.0, -hl to 0.0)
    float pumpW = hw * 0.95f;
    float pumpL = hl * 0.85f;
    float pumpH = 7.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hl},
        Vec3{-hw + pumpW, pumpH, -hl + pumpL},
        concMat);

    // Parapet roof rim on pump house
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.15f, pumpH, -hl - 0.15f},
        Vec3{-hw + pumpW + 0.15f, pumpH + 0.6f, -hl + pumpL + 0.15f},
        steelMat);

    // High clerestory ribbon windows along pump hall
    BuildingComponents::generateWindow(outVerts, outIndices,
        Vec3{-hw + pumpW * 0.5f, pumpH - 1.2f, -hl - 0.02f},
        pumpW * 0.75f, 1.4f,
        Vec3{0, 0, -1}, Vec3{1, 0, 0},
        WindowType::RibbonStrip, GlassType::ClearFloat);

    // Industrial roll-up equipment door and service portal
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{-hw + pumpW * 0.5f, 0.0f, -hl + pumpL + 0.02f},
        3.4f, 4.2f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::IndustrialRollUp, true);

    // 3. Twin Circular Clarifier / Settling Basins (right side: +X, along Z)
    float basinR = 5.6f;
    float basinWallH = 2.4f;
    float basinZ0 = -hl * 0.45f;
    float basinZ1 = hl * 0.45f;
    float basinX = hw * 0.45f;

    for (float bZ : {basinZ0, basinZ1}) {
        // Concrete basin outer cylinder shell
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{basinX, 0.0f, bZ},
            Vec3{basinX, basinWallH, bZ},
            basinR, basinR, 24, concMat, true, false);

        // Water surface inside basin
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{basinX, 0.0f, bZ},
            Vec3{basinX, basinWallH - 0.25f, bZ},
            basinR * 0.94f, basinR * 0.94f, 24, waterMat, true, true);

        // Center clarifier pivot pedestal
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{basinX, 0.0f, bZ},
            Vec3{basinX, basinWallH + 0.8f, bZ},
            0.65f, 0.65f, 16, steelMat, true, true);

        // Rotating clarifier scraper bridge truss spanning across basin
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{basinX - basinR * 0.98f, basinWallH + 0.4f, bZ - 0.45f},
            Vec3{basinX + basinR * 0.98f, basinWallH + 0.55f, bZ + 0.45f},
            steelMat);
        // Safety handrails along bridge
        for (float side : {-1.0f, 1.0f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{basinX - basinR * 0.98f, basinWallH + 0.55f, bZ + side * 0.42f - 0.03f},
                Vec3{basinX + basinR * 0.98f, basinWallH + 1.55f, bZ + side * 0.42f + 0.03f},
                safetyYellow);
        }
    }

    // 4. Multi-Stage Flocculation & Chemical Dosing Basin (rear left: -hw to 0.0, 0.0 to +hl)
    float flocW = hw * 0.9f;
    float flocL = hl * 0.75f;
    float flocH = 2.2f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, 0.5f},
        Vec3{-hw + flocW, flocH, 0.5f + flocL},
        concMat);

    // Water surface in flocculation tanks (3 partitioned cells)
    for (int cell = 0; cell < 3; ++cell) {
        float cz0 = 0.8f + cell * (flocL / 3.0f);
        float cz1 = cz0 + (flocL / 3.0f) - 0.3f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 0.4f, 0.0f, cz0},
            Vec3{-hw + flocW - 0.4f, flocH - 0.2f, cz1},
            waterMat);
    }

    // Overhead service walkway catwalk across flocculation tanks
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + flocW * 0.5f - 0.6f, flocH, 0.5f},
        Vec3{-hw + flocW * 0.5f + 0.6f, flocH + 0.15f, 0.5f + flocL},
        steelMat);
    for (float side : {-1.0f, 1.0f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + flocW * 0.5f + side * 0.58f - 0.03f, flocH + 0.15f, 0.5f},
            Vec3{-hw + flocW * 0.5f + side * 0.58f + 0.03f, flocH + 1.15f, 0.5f + flocL},
            safetyYellow);
    }

    // 5. Elevated Chemical Sludge Storage Silo on heavy steel legs
    float siloX = -hw + flocW * 0.5f;
    float siloZ = 0.5f + flocL + 3.2f;
    float siloH = 9.0f;
    float siloR = 2.2f;
    float legH = 3.6f;

    // 4 structural tubular steel legs
    for (float sx : {-1.4f, 1.4f}) {
        for (float sz : {-1.4f, 1.4f}) {
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{siloX + sx, 0.0f, siloZ + sz},
                Vec3{siloX + sx * 0.8f, legH, siloZ + sz * 0.8f},
                0.16f, 0.16f, 12, steelMat, true, true);
        }
    }

    // Conical hopper bottom
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{siloX, legH, siloZ},
        Vec3{siloX, legH + 1.6f, siloZ},
        0.5f, siloR, 18, steelMat, true, true);

    // Cylindrical silo tank
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{siloX, legH + 1.6f, siloZ},
        Vec3{siloX, siloH, siloZ},
        siloR, siloR, 18, steelMat, true, true);

    // Conical silo roof cap
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{siloX, siloH, siloZ},
        Vec3{siloX, siloH + 1.4f, siloZ},
        siloR, 0.1f, 18, steelMat, true, true);

    // 6. Interconnecting Large-Diameter Stainless Process Pipe Manifolds
    // Pipe connecting pump house to clarifier basin #1
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{-hw + pumpW, 2.5f, -hl * 0.45f},
        Vec3{basinX - basinR, 2.5f, -hl * 0.45f},
        0.32f, 0.32f, 16, pipeMat, true, true);
    // Pipe connecting clarifier #1 to clarifier #2
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{basinX, 2.2f, basinZ0 + basinR},
        Vec3{basinX, 2.2f, basinZ1 - basinR},
        0.28f, 0.28f, 16, pipeMat, true, true);
}

} // namespace procedural::buildings
