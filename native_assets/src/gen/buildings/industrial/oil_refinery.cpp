#include "WarehouseGen.h"
#include "../BuildingComponents.h"
#include <cmath>
#include <algorithm>

namespace procedural::buildings {

void WarehouseGen::generateRefinery(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(44.0f, std::max(params_.width, params_.length));
    float d = std::max(32.0f, std::min(params_.width, params_.length));
    float h = std::max(8.0f, params_.height);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallConcrete = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t tarmacMat = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t silverAlum = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t darkMetal = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t champAlum = static_cast<uint32_t>(MaterialSlot::WallChampagneAluminum);
    uint32_t safetyYellow = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t signOrange = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t signRed = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t hvacMat = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t doorMetal = static_cast<uint32_t>(MaterialSlot::DoorWoodMetal);

    // 1. Reinforced Process Pad & Chemical Containment Dike
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.0f, -0.4f, -hd - 4.0f},
        Vec3{ hw + 4.0f,  0.0f,  hd + 4.0f},
        tarmacMat);

    // Perimeter Secondary Containment Concrete Dike Wall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.5f, 0.0f, -hd - 0.5f},
        Vec3{ hw + 0.5f, 0.75f, hd + 0.5f},
        foundMat);

    // 2. Tall Primary Distillation / Fractionation Column (Left Column)
    float col1X = -hw + 10.0f;
    float col1Z = -2.0f;
    float col1R = 2.4f;
    float col1H = std::max(26.0f, h * 2.2f);
    int segs = 12;

    // Cylinder shell
    for (int i = 0; i < segs; ++i) {
        float a0 = (static_cast<float>(i) / segs) * 6.28318f;
        float a1 = (static_cast<float>(i + 1) / segs) * 6.28318f;
        float x0 = col1X + col1R * std::cos(a0);
        float z0 = col1Z + col1R * std::sin(a0);
        float x1 = col1X + col1R * std::cos(a1);
        float z1 = col1Z + col1R * std::sin(a1);

        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{std::min(x0, x1), 0.75f, std::min(z0, z1)},
            Vec3{std::max(x0, x1) + 0.1f, col1H, std::max(z0, z1) + 0.1f},
            silverAlum);
    }

    // Circular Maintenance Catwalk Platforms at 4 elevation tiers
    for (float py : {col1H * 0.25f, col1H * 0.5f, col1H * 0.75f, col1H - 1.5f}) {
        // Catwalk Deck
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{col1X - col1R - 1.2f, py, col1Z - col1R - 1.2f},
            Vec3{col1X + col1R + 1.2f, py + 0.18f, col1Z + col1R + 1.2f},
            frameMat);
        // Safety Railing
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{col1X - col1R - 1.2f, py + 0.18f, col1Z - col1R - 1.2f},
            Vec3{col1X + col1R + 1.2f, py + 1.15f, col1Z - col1R - 1.05f},
            safetyYellow);
    }
    // Vertical Access Ladder Cage running up column
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{col1X + col1R + 0.2f, 0.75f, col1Z - 0.4f},
        Vec3{col1X + col1R + 0.9f, col1H - 1.0f, col1Z + 0.4f},
        frameMat);

    // 3. Secondary Vacuum Distillation Column (Center-Left)
    float col2X = -hw + 18.0f;
    float col2Z = 4.0f;
    float col2R = 1.8f;
    float col2H = col1H * 0.75f;
    for (int i = 0; i < segs; ++i) {
        float a0 = (static_cast<float>(i) / segs) * 6.28318f;
        float a1 = (static_cast<float>(i + 1) / segs) * 6.28318f;
        float x0 = col2X + col2R * std::cos(a0);
        float z0 = col2Z + col2R * std::sin(a0);
        float x1 = col2X + col2R * std::cos(a1);
        float z1 = col2Z + col2R * std::sin(a1);

        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{std::min(x0, x1), 0.75f, std::min(z0, z1)},
            Vec3{std::max(x0, x1) + 0.1f, col2H, std::max(z0, z1) + 0.1f},
            champAlum);
    }
    // Overhead condenser pipe loop
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{col2X - 0.5f, col2H, col2Z - 0.5f},
        Vec3{col2X + 0.5f, col2H + 2.4f, col2Z + 0.5f},
        darkMetal);

    // 4. Pressurized Gas Spherical Storage Tanks (Horton Spheres on Stilts)
    for (float sx : {hw - 14.0f, hw - 5.0f}) {
        float sz = -hd + 10.0f;
        float sR = 4.2f;
        float sCenterY = 7.5f;

        // Spherical pressurized tank body
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx - sR, sCenterY - sR, sz - sR},
            Vec3{sx + sR, sCenterY + sR, sz + sR},
            silverAlum);

        // Equatorial reinforcement ring band
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx - sR - 0.25f, sCenterY - 0.35f, sz - sR - 0.25f},
            Vec3{sx + sR + 0.25f, sCenterY + 0.35f, sz + sR + 0.25f},
            frameMat);

        // 6 Structural Steel Support Stilts with Safety Yellow crash collars
        for (int i = 0; i < 6; ++i) {
            float ang = (static_cast<float>(i) / 6.0f) * 6.28318f;
            float legX = sx + (sR * 0.85f) * std::cos(ang);
            float legZ = sz + (sR * 0.85f) * std::sin(ang);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{legX - 0.22f, 0.75f, legZ - 0.22f},
                Vec3{legX + 0.22f, sCenterY, legZ + 0.22f},
                frameMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{legX - 0.3f, 0.75f, legZ - 0.3f},
                Vec3{legX + 0.3f, 2.0f, legZ + 0.3f},
                safetyYellow);
        }
    }

    // 5. Multi-Tier Industrial Pipe Rack Gantry
    float rackZ = 0.0f;
    float rackY = 4.5f;
    float rackSpan = w - 8.0f;
    // Structural steel truss gantry
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 4.0f, rackY, rackZ - 1.2f},
        Vec3{ hw - 4.0f, rackY + 0.35f, rackZ + 1.2f},
        frameMat);
    // 4 Parallel process pipelines carried on gantry
    // Pipe 1 (Silver Steam)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 3.0f, rackY + 0.4f, rackZ - 0.8f},
        Vec3{ hw - 3.0f, rackY + 0.95f, rackZ - 0.4f},
        silverAlum);
    // Pipe 2 (Process Orange)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 3.0f, rackY + 0.4f, rackZ - 0.25f},
        Vec3{ hw - 3.0f, rackY + 1.1f, rackZ + 0.25f},
        signOrange);
    // Pipe 3 (Crude Red)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 3.0f, rackY + 0.4f, rackZ + 0.4f},
        Vec3{ hw - 3.0f, rackY + 0.9f, rackZ + 0.8f},
        signRed);
    // Gantry Vertical Trestle Bents
    for (float tx = -hw + 6.0f; tx <= hw - 6.0f; tx += 10.0f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{tx - 0.25f, 0.75f, rackZ - 1.2f},
            Vec3{tx + 0.25f, rackY, rackZ + 1.2f},
            frameMat);
    }

    // 6. Safety Flare Stack Pylon
    float flareX = hw + 1.5f;
    float flareZ = hd - 4.0f;
    float flareH = 30.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{flareX - 0.45f, 0.0f, flareZ - 0.45f},
        Vec3{flareX + 0.45f, flareH, flareZ + 0.45f},
        darkMetal);
    // Top Gas Burner Tip & Flare Flame Shield
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{flareX - 0.9f, flareH - 1.2f, flareZ - 0.9f},
        Vec3{flareX + 0.9f, flareH + 0.5f, flareZ + 0.9f},
        signRed);

    // 7. Blast-Resistant Concrete Control Bunker & Compressor House
    float bunkW = 14.0f;
    float bunkD = 10.0f;
    float bunkH = 4.8f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-bunkW * 0.5f, 0.75f, hd - bunkD - 1.0f},
        Vec3{ bunkW * 0.5f, 0.75f + bunkH, hd - 1.0f},
        wallConcrete);
    // Heavy blast door with yellow safety header
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-1.5f, 0.75f, hd - 0.95f},
        Vec3{ 1.5f, 3.4f, hd - 0.75f},
        doorMetal);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-1.6f, 3.4f, hd - 0.98f},
        Vec3{ 1.6f, 3.7f, hd - 0.7f},
        safetyYellow);
}

} // namespace procedural::buildings
