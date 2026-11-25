#include "WarehouseGen.h"
#include "../BuildingComponents.h"
#include <cmath>
#include <algorithm>

namespace procedural::buildings {

void WarehouseGen::generateSteelMill(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(48.0f, std::max(params_.width, params_.length));
    float d = std::max(30.0f, std::min(params_.width, params_.length));
    float h = std::max(12.0f, params_.height * 1.4f);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallConcrete = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t tarmacMat = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t silverAlum = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t darkMetal = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t cortenSteel = static_cast<uint32_t>(MaterialSlot::WallCorten);
    uint32_t safetyYellow = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t doorMetal = static_cast<uint32_t>(MaterialSlot::DoorWoodMetal);
    uint32_t signRed = static_cast<uint32_t>(MaterialSlot::TrackRed);

    // 1. Heavy Industrial Steel Mill Foundation & Scrap Yard
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 6.0f, -0.4f, -hd - 4.0f},
        Vec3{ hw + 6.0f,  0.0f,  hd + 6.0f},
        tarmacMat);

    // Sunken Heavy Torpedo Rail Tracks along front yard
    for (float rz : {hd + 1.5f, hd + 3.0f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw, 0.0f, rz - 0.12f},
            Vec3{ hw, 0.18f, rz + 0.12f},
            frameMat);
    }

    // 2. Main Foundry & Casting Hall (Left / Center Volume)
    float hallW = w * 0.62f;
    float hallD = d * 0.85f;
    float hallX = -hw + hallW * 0.5f + 1.0f;
    float hallZ = 0.0f;

    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hallX - hallW * 0.5f, 0.0f, hallZ - hallD * 0.5f},
        Vec3{hallX + hallW * 0.5f, h,    hallZ + hallD * 0.5f},
        cortenSteel);

    // Monitor Roof with Continuous Ventilation Louvers atop Hall
    float monitorW = hallW * 0.45f;
    float monitorH = 3.6f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hallX - monitorW * 0.5f, h, hallZ - hallD * 0.5f - 0.2f},
        Vec3{hallX + monitorW * 0.5f, h + monitorH, hallZ + hallD * 0.5f + 0.2f},
        darkMetal);
    // Monitor Louver Openings
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hallX - monitorW * 0.5f - 0.2f, h + 0.8f, hallZ - hallD * 0.5f},
        Vec3{hallX - monitorW * 0.5f + 0.1f, h + 2.8f, hallZ + hallD * 0.5f},
        frameMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hallX + monitorW * 0.5f - 0.1f, h + 0.8f, hallZ - hallD * 0.5f},
        Vec3{hallX + monitorW * 0.5f + 0.2f, h + 2.8f, hallZ + hallD * 0.5f},
        frameMat);

    // Heavy Industrial Roll-Up Equipment Portal (Molten Ingot / Ladle Exit)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hallX - 4.2f, 0.0f, hallZ + hallD * 0.5f + 0.05f},
        Vec3{hallX + 4.2f, 6.2f, hallZ + hallD * 0.5f + 0.25f},
        doorMetal);
    // Safety Yellow & Black Diagonal Hazard Stripe Frame
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hallX - 4.6f, 0.0f, hallZ + hallD * 0.5f + 0.2f},
        Vec3{hallX - 4.2f, 6.6f, hallZ + hallD * 0.5f + 0.4f},
        safetyYellow);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hallX + 4.2f, 0.0f, hallZ + hallD * 0.5f + 0.2f},
        Vec3{hallX + 4.6f, 6.6f, hallZ + hallD * 0.5f + 0.4f},
        safetyYellow);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hallX - 4.6f, 6.2f, hallZ + hallD * 0.5f + 0.2f},
        Vec3{hallX + 4.6f, 6.6f, hallZ + hallD * 0.5f + 0.4f},
        safetyYellow);

    // 3. Blast Furnace Hot Stove Cylinders (Right Section)
    float stove1X = hw - 10.0f;
    float stove1Z = -4.0f;
    float stove2X = hw - 10.0f;
    float stove2Z = 5.0f;
    float stoveR = 3.4f;
    float stoveH = h * 1.5f; // 24m tall
    int segs = 14;

    for (float sz : {stove1Z, stove2Z}) {
        // Cylindrical blast stove shell
        for (int i = 0; i < segs; ++i) {
            float a0 = (static_cast<float>(i) / segs) * 6.28318f;
            float a1 = (static_cast<float>(i + 1) / segs) * 6.28318f;
            float x0 = stove1X + stoveR * std::cos(a0);
            float z0 = sz + stoveR * std::sin(a0);
            float x1 = stove1X + stoveR * std::cos(a1);
            float z1 = sz + stoveR * std::sin(a1);

            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{std::min(x0, x1), 0.0f, std::min(z0, z1)},
                Vec3{std::max(x0, x1) + 0.1f, stoveH, std::max(z0, z1) + 0.1f},
                darkMetal);
        }
        // Hemispherical Dome Cap
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{stove1X - stoveR, stoveH, sz - stoveR},
            Vec3{stove1X + stoveR, stoveH + 2.8f, sz + stoveR},
            silverAlum);
    }

    // Downcomer Large Gas Duct Pipe connecting stoves to hall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hallX + hallW * 0.5f, h * 0.85f, -1.0f},
        Vec3{stove1X, h * 0.85f + 1.8f, 1.0f},
        silverAlum);

    // 4. Enclosed Inclined Ore & Scrap Conveyor Gallery
    float convStartX = -hw + 2.0f;
    float convEndX = hallX - hallW * 0.1f;
    float convY0 = 0.5f;
    float convY1 = h * 0.85f;
    float convZ = -hd + 3.0f;
    // Slanted conveyor truss box
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{convStartX, convY0, convZ - 1.2f},
        Vec3{convEndX,   convY1, convZ + 1.2f},
        darkMetal);
    // Conveyor Ground Trestle Support
    float convMidX = (convStartX + convEndX) * 0.5f;
    float convMidY = (convY0 + convY1) * 0.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{convMidX - 0.4f, 0.0f, convZ - 1.2f},
        Vec3{convMidX + 0.4f, convMidY, convZ + 1.2f},
        frameMat);

    // 5. Heavy Raw Scrap Storage Push-Wall Bays
    float scrapX = -hw + 6.0f;
    float scrapZ = hd - 4.0f;
    // Concrete Push-Walls
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{scrapX - 6.0f, 0.0f, scrapZ - 4.0f},
        Vec3{scrapX + 6.0f, 3.2f, scrapZ - 3.4f},
        wallConcrete);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{scrapX - 6.0f, 0.0f, scrapZ - 4.0f},
        Vec3{scrapX - 5.4f, 3.2f, scrapZ + 4.0f},
        wallConcrete);
    // Raw Iron Scrap Mound inside bay
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{scrapX - 5.0f, 0.0f, scrapZ - 3.0f},
        Vec3{scrapX + 5.0f, 2.4f, scrapZ + 3.0f},
        frameMat);
}

} // namespace procedural::buildings
