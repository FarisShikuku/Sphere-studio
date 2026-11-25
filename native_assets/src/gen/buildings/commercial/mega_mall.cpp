#include "TowerGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <cmath>

namespace procedural::buildings {

void TowerGen::generateMegaMall(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(52.0f, params_.footprintWidth);
    float d = std::max(40.0f, params_.footprintDepth);
    int32_t lvlCount = std::max(3, params_.stories);
    float h = lvlCount * 4.2f; // e.g. 16.8m for 4 levels
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallConcrete = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t wallLimestone = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t glassGold = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t tarmacMat = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t champAlum = static_cast<uint32_t>(MaterialSlot::WallChampagneAluminum);
    uint32_t silverAlum = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t blackAlum = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t signOrange = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t signRed = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t hvacMat = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t safetyYellow = static_cast<uint32_t>(MaterialSlot::SafetyYellow);

    // 1. Sprawling Sited Apron & Parking Forecourt
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 8.0f, -0.3f, -hd - 4.0f},
        Vec3{ hw + 8.0f,  0.0f,  hd + 10.0f},
        tarmacMat);

    // Parking bay markings in forecourt
    for (float px = -hw + 4.0f; px < hw - 4.0f; px += 3.8f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px, 0.01f, hd + 3.0f},
            Vec3{px + 0.15f, 0.03f, hd + 8.5f},
            silverAlum);
    }

    // 2. Elevated Promenade Plinth Foundation
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.0f, 0.0f, -hd - 1.0f},
        Vec3{ hw + 1.0f, 0.6f,  hd + 1.5f},
        foundMat);

    // 3. Central Grand Concourse & Main Building Body
    float concourseW = 20.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-concourseW * 0.5f, 0.6f, -hd},
        Vec3{ concourseW * 0.5f, h + 1.5f, hd + 2.0f},
        glassTinted);

    // Concourse Structural Steel Frame Mullions
    for (float mx = -concourseW * 0.5f; mx <= concourseW * 0.5f + 0.1f; mx += 4.0f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mx - 0.2f, 0.6f, hd + 2.0f},
            Vec3{mx + 0.2f, h + 1.5f, hd + 2.3f},
            blackAlum);
    }

    // 4. Monumental Arched Glass Barrel-Vault Skylight along Central Concourse
    float vaultR = concourseW * 0.5f;
    float vaultBaseY = h + 1.5f;
    int segments = 12;
    for (int i = 0; i < segments; ++i) {
        float a0 = (static_cast<float>(i) / segments) * 3.14159f;
        float a1 = (static_cast<float>(i + 1) / segments) * 3.14159f;
        float x0 = -vaultR * std::cos(a0);
        float y0 = vaultBaseY + vaultR * 0.5f * std::sin(a0);
        float x1 = -vaultR * std::cos(a1);
        float y1 = vaultBaseY + vaultR * 0.5f * std::sin(a1);

        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{std::min(x0, x1), std::min(y0, y1), -hd + 2.0f},
            Vec3{std::max(x0, x1) + 0.1f, std::max(y0, y1) + 0.15f, hd + 1.0f},
            glassMat);
    }

    // Barrel Vault Steel Arch Ribs (7 arch ribs)
    for (float rz = -hd + 3.0f; rz <= hd + 0.1f; rz += (d - 4.0f) / 6.0f) {
        for (int i = 0; i <= segments; ++i) {
            float ang = (static_cast<float>(i) / segments) * 3.14159f;
            float rx = -vaultR * std::cos(ang);
            float ry = vaultBaseY + vaultR * 0.5f * std::sin(ang);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{rx - 0.25f, ry - 0.25f, rz - 0.2f},
                Vec3{rx + 0.25f, ry + 0.25f, rz + 0.2f},
                frameMat);
        }
    }

    // 5. West Anchor Department Store Wing (Nordstrom / Bloomingdale's style)
    float westW = (w - concourseW) * 0.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.6f, -hd},
        Vec3{-concourseW * 0.5f, h, hd},
        champAlum);

    // West Anchor Decorative Vertical Architectural Fins
    for (float fx = -hw + 2.0f; fx < -concourseW * 0.5f - 1.0f; fx += 2.8f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{fx - 0.15f, 0.6f, hd},
            Vec3{fx + 0.15f, h, hd + 0.45f},
            blackAlum);
    }
    // West Anchor Brand Header Sign
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 2.5f, h - 2.5f, hd + 0.46f},
        Vec3{-concourseW * 0.5f - 2.5f, h - 0.8f, hd + 0.65f},
        signRed);

    // 6. East Anchor Department Store Wing with Double-Height Curtain Wall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{concourseW * 0.5f, 0.6f, -hd},
        Vec3{hw, h, hd},
        wallLimestone);

    // East Wing Glazed Display Showcase
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{concourseW * 0.5f + 2.0f, 1.0f, hd},
        Vec3{hw - 2.0f, h * 0.65f, hd + 0.8f},
        glassGold);
    // East Wing Store Sign Band
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{concourseW * 0.5f + 3.0f, h - 2.8f, hd + 0.1f},
        Vec3{hw - 3.0f, h - 1.0f, hd + 0.35f},
        signOrange);

    // 7. Attached Multi-Deck Structured Parking Garage (Rear/Side)
    float parkW = 22.0f;
    float parkD = 18.0f;
    float parkH = 10.5f; // 3 levels
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, 0.6f, -hd - parkD + 2.0f},
        Vec3{-hw - 2.0f + parkW, parkH, -hd + 2.0f},
        wallConcrete);

    // Parking Deck Open Air Spandrels & Slabs
    for (float py = 3.5f; py < parkH; py += 3.5f) {
        // Floor slab edge
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 2.2f, py - 0.3f, -hd - parkD + 1.8f},
            Vec3{-hw - 1.8f + parkW, py, -hd + 2.2f},
            silverAlum);
        // Vehicle guard rail
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw - 2.1f, py, -hd - parkD + 1.9f},
            Vec3{-hw - 2.1f, py + 1.0f, -hd + 2.1f},
            frameMat);
    }
    // Parking Vehicle Access Ramp on end
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f + parkW, 0.6f, -hd - parkD + 3.0f},
        Vec3{-hw - 2.0f + parkW + 4.5f, 4.0f, -hd - 2.0f},
        wallConcrete);

    // 8. Rooftop Cinema / Entertainment Multiplex Pavilion (East Rooftop)
    float cineW = 16.0f;
    float cineD = 16.0f;
    float cineH = 5.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - cineW - 1.0f, h, -cineD * 0.5f},
        Vec3{hw - 1.0f, h + cineH, cineD * 0.5f},
        blackAlum);
    // Cinema Marquee Illuminated Sign Box
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{hw - cineW + 1.0f, h + cineH - 2.2f, cineD * 0.5f + 0.1f},
        Vec3{hw - 3.0f, h + cineH - 0.6f, cineD * 0.5f + 0.4f},
        signRed);

    // 9. Grand Entrance Portico & Plaza Canopies
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-12.0f, 4.8f, hd + 1.5f},
        Vec3{ 12.0f, 5.3f, hd + 7.5f},
        silverAlum);

    // 4 Monumental Portico Columns
    for (float cx = -9.0f; cx <= 9.1f; cx += 6.0f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx - 0.35f, 0.6f, hd + 7.0f},
            Vec3{cx + 0.35f, 4.8f, hd + 7.5f},
            champAlum);
    }

    // Grand Sliding / Revolving Entrance Glass Doors
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{-4.0f, 0.6f, hd + 2.0f},
        4.0f, 3.4f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true);
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{ 4.0f, 0.6f, hd + 2.0f},
        4.0f, 3.4f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true);

    // 10. Multi-Tenant Roadside Pylon Directory Totem
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{concourseW * 0.5f + 8.0f, 0.0f, hd + 8.0f},
        Vec3{concourseW * 0.5f + 9.5f, 8.5f, hd + 8.8f},
        blackAlum);
    // Totem Illuminated Brand Header
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{concourseW * 0.5f + 7.8f, 7.0f, hd + 7.9f},
        Vec3{concourseW * 0.5f + 9.7f, 8.4f, hd + 8.9f},
        signOrange);

    // 11. Rooftop Chiller Plants & Cooling Towers
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 4.0f, h, -hd + 4.0f},
        Vec3{-concourseW * 0.5f - 3.0f, h + 2.4f, -hd + 10.0f},
        hvacMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{concourseW * 0.5f + 4.0f, h, -hd + 4.0f},
        Vec3{hw - 4.0f, h + 2.4f, -hd + 10.0f},
        hvacMat);
}

} // namespace procedural::buildings
