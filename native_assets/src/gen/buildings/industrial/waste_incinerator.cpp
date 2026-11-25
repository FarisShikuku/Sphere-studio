#include "WarehouseGen.h"
#include "../BuildingComponents.h"
#include <cmath>

namespace procedural::buildings {

void WarehouseGen::generateWasteIncinerator(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float l = std::max(42.0f, params_.length);
    float w = std::max(28.0f, params_.width);
    float hl = l * 0.5f;
    float hw = w * 0.5f;

    uint32_t concMat     = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t cortenMat   = static_cast<uint32_t>(MaterialSlot::WallCorten);
    uint32_t darkSlate   = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t silverAlum  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t safetyYellow = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t hvacMetal   = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t stackRed    = static_cast<uint32_t>(MaterialSlot::TrackRed);

    // 1. Foundation slab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, -0.5f, -hl - 2.0f},
        Vec3{hw + 2.0f, 0.0f, hl + 2.0f},
        foundMat);

    // 2. High-Capacity Waste Boiler & Furnace Combustion Tower (Center rear: height 28m)
    float towerW = hw * 1.1f;
    float towerL = hl * 0.9f;
    float towerH = 28.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-towerW * 0.5f, 0.0f, -hl},
        Vec3{towerW * 0.5f, towerH, -hl + towerL},
        cortenMat);

    // Architectural sloped roof monitor cap on the furnace tower
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-towerW * 0.5f - 0.2f, towerH, -hl - 0.2f},
        Vec3{towerW * 0.5f + 0.2f, towerH + 2.2f, -hl + towerL + 0.2f},
        darkSlate);

    // 3. Front Refuse Tipping Hall & Truck Reception Bays (height 10.5m)
    float tipW = hw * 1.5f;
    float tipL = hl * 0.85f;
    float tipH = 10.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-tipW * 0.5f, 0.0f, -hl + towerL},
        Vec3{tipW * 0.5f, tipH, hl},
        concMat);

    // 3 Sunken Waste Intake Bays with Heavy Industrial Roll-Up Doors
    float baySpacing = tipW * 0.28f;
    for (float bx : {-baySpacing, 0.0f, baySpacing}) {
        // Yellow safety hazard surround
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{bx - 2.2f, 0.0f, hl},
            Vec3{bx + 2.2f, 5.2f, hl + 0.15f},
            safetyYellow);
        // Heavy corrugated roll-up door
        BuildingComponents::generateDoor(outVerts, outIndices,
            Vec3{bx, 0.0f, hl + 0.16f},
            3.8f, 4.8f,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            DoorType::IndustrialRollUp, true);
    }

    // 4. Soaring Continuous Emission Flue Gas Stack (Height 36m, Left rear: -X, -Z)
    float stackX = -hw * 0.68f;
    float stackZ = -hl + 3.0f;
    float stackH = 36.0f;

    // Stack concrete cylinder body (white/grey)
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{stackX, 0.0f, stackZ},
        Vec3{stackX, stackH, stackZ},
        2.2f, 1.4f, 24, concMat, true, true);

    // ICAO aviation warning red stripe bands near top
    for (float ry : {stackH - 8.0f, stackH - 4.0f, stackH}) {
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{stackX, ry - 1.8f, stackZ},
            Vec3{stackX, ry, stackZ},
            1.55f, 1.45f, 24, stackRed, false, false);
    }

    // Continuous emissions monitoring circular platform catwalk
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{stackX, stackH * 0.65f, stackZ},
        Vec3{stackX, stackH * 0.65f + 0.25f, stackZ},
        2.8f, 2.8f, 24, silverAlum, true, true);
    // Safety railing around stack catwalk
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{stackX, stackH * 0.65f + 0.25f, stackZ},
        Vec3{stackX, stackH * 0.65f + 1.25f, stackZ},
        2.8f, 2.8f, 24, safetyYellow, false, false);

    // 5. Overhead Enclosed Conveyor Bridge for Recycled Scrap Metals (Right side, +X)
    float convX0 = towerW * 0.5f;
    float convX1 = hw * 1.35f;
    float convZ  = -hl + towerL * 0.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{convX0, 12.0f, convZ - 1.1f},
        Vec3{convX1, 14.5f, convZ + 1.1f},
        silverAlum);

    // Bottom-ash & metal loading silo
    float ashSiloX = convX1 + 1.5f;
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{ashSiloX, 4.0f, convZ},
        Vec3{ashSiloX, 16.0f, convZ},
        2.8f, 2.8f, 18, concMat, true, true);
    // 4 structural stilt legs for truck drive-under loading
    for (float sx : {-1.8f, 1.8f}) {
        for (float sz : {-1.8f, 1.8f}) {
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{ashSiloX + sx, 0.0f, convZ + sz},
                Vec3{ashSiloX + sx, 4.0f, convZ + sz},
                0.22f, 0.22f, 10, darkSlate, true, true);
        }
    }

    // 6. Rooftop Air-Cooled Steam Condensers (on tipping hall roof)
    for (float cx : {-tipW * 0.25f, tipW * 0.25f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx - 2.8f, tipH + 0.2f, 0.0f},
            Vec3{cx + 2.8f, tipH + 2.2f, hl * 0.7f},
            hvacMetal);
        // Condenser cooling fan hoods
        for (float fz = 1.0f; fz < hl * 0.65f; fz += 3.0f) {
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{cx, tipH + 2.2f, fz},
                Vec3{cx, tipH + 3.0f, fz},
                1.1f, 1.1f, 16, hvacMetal, false, true);
        }
    }
}

} // namespace procedural::buildings
