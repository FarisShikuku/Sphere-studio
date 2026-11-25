#include "WarehouseGen.h"
#include "../BuildingComponents.h"
#include <cmath>

namespace procedural::buildings {

void WarehouseGen::generateDataCenter(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float l = std::max(42.0f, params_.length);
    float w = std::max(28.0f, params_.width);
    float h = 9.5f;
    float hl = l * 0.5f;
    float hw = w * 0.5f;

    uint32_t basaltSlate = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t silverAlum  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t hvacMetal   = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t concMat     = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t roofMat     = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t glassMat    = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t safetyYellow = static_cast<uint32_t>(MaterialSlot::SafetyYellow);

    // 1. Foundation slab
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, -0.4f, -hl - 2.0f},
        Vec3{hw + 2.0f, 0.0f, hl + 2.0f},
        foundMat);

    // 2. Main High-Security Server Data Hall (Dark Basalt Composite Cladding)
    float hallW = hw * 1.5f;
    float hallL = hl * 1.6f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hallW * 0.5f, 0.0f, -hallL * 0.5f},
        Vec3{hallW * 0.5f, h, hallL * 0.5f},
        basaltSlate);

    // Architectural silver reveal pilasters & recessed LED lighting channels
    int pilasters = 9;
    for (int p = 0; p < pilasters; ++p) {
        float pz = -hallL * 0.5f + p * (hallL / (pilasters - 1));
        // Left & right pilasters
        for (float side : {-1.0f, 1.0f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{side * hallW * 0.5f - 0.12f, 0.0f, pz - 0.18f},
                Vec3{side * hallW * 0.5f + 0.12f, h + 0.4f, pz + 0.18f},
                silverAlum);
        }
    }

    // Parapet roof screen around server hall
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hallW * 0.5f - 0.15f, h, -hallL * 0.5f - 0.15f},
        Vec3{hallW * 0.5f + 0.15f, h + 2.2f, hallL * 0.5f + 0.15f},
        silverAlum);

    // 3. Massive Rooftop Modular Chiller Banks & Adiabatic Cooling Towers
    int chillerCount = 6;
    for (int c = 0; c < chillerCount; ++c) {
        float cz = -hallL * 0.38f + c * (hallL * 0.76f / (chillerCount - 1));
        for (float cx : {-hallW * 0.25f, hallW * 0.25f}) {
            // Chiller condenser cabinet
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{cx - 1.4f, h + 0.2f, cz - 1.1f},
                Vec3{cx + 1.4f, h + 1.8f, cz + 1.1f},
                hvacMetal);
            // Twin top exhaust fan cowls
            for (float fx : {-0.7f, 0.7f}) {
                BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                    Vec3{cx + fx, h + 1.8f, cz},
                    Vec3{cx + fx, h + 2.4f, cz},
                    0.55f, 0.55f, 14, hvacMetal, false, true);
            }
        }
    }

    // 4. Backup Emergency Power Corridor: 4 Containerized Diesel Generators (Right side, +X)
    float genX = hallW * 0.5f + 3.2f;
    for (int g = 0; g < 4; ++g) {
        float gz = -hallL * 0.4f + g * (hallL * 0.8f / 3.0f);
        // Enclosed acoustic generator container pod
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{genX - 1.4f, 0.0f, gz - 2.2f},
            Vec3{genX + 1.4f, 3.4f, gz + 2.2f},
            silverAlum);

        // Heavy vertical exhaust silencer stack
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{genX + 0.6f, 3.4f, gz + 1.4f},
            Vec3{genX + 0.6f, 6.2f, gz + 1.4f},
            0.24f, 0.24f, 12, hvacMetal, true, true);

        // Rain flap cap on exhaust
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{genX + 0.3f, 6.2f, gz + 1.1f},
            Vec3{genX + 0.9f, 6.35f, gz + 1.7f},
            silverAlum);

        // Safety yellow fuel line & barrier
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{genX - 1.6f, 0.0f, gz - 2.4f},
            Vec3{genX + 1.6f, 0.35f, gz - 2.2f},
            safetyYellow);
    }

    // 5. High-Voltage Electrical Substation Pad & Step-Down Transformers (Rear, -Z)
    float transZ = -hallL * 0.5f - 3.2f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hallW * 0.4f, 0.0f, transZ - 1.8f},
        Vec3{hallW * 0.4f, 0.25f, transZ + 1.8f},
        concMat);
    for (float tx : {-hallW * 0.25f, 0.0f, hallW * 0.25f}) {
        // Transformer body
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{tx - 1.1f, 0.25f, transZ - 0.9f},
            Vec3{tx + 1.1f, 2.6f, transZ + 0.9f},
            hvacMetal);
        // Ceramic high-voltage insulator bushings
        for (float bx : {-0.6f, 0.0f, 0.6f}) {
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{tx + bx, 2.6f, transZ},
                Vec3{tx + bx, 3.6f, transZ},
                0.14f, 0.08f, 10, static_cast<uint32_t>(MaterialSlot::FrameMetal), true, true);
        }
    }

    // 6. Security Operations & Network Operations Center (NOC) Entrance Vestibule (Front, +Z)
    float nocW = 10.0f;
    float nocD = 4.2f;
    float nocH = 4.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-nocW * 0.5f, 0.0f, hallL * 0.5f},
        Vec3{nocW * 0.5f, nocH, hallL * 0.5f + nocD},
        concMat);

    // Bullet-resistant security glass curtain wall at NOC entrance
    BuildingComponents::generateWindow(outVerts, outIndices,
        Vec3{0.0f, nocH * 0.5f, hallL * 0.5f + nocD + 0.02f},
        nocW * 0.65f, 2.6f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        WindowType::PictureGlass, GlassType::SolarTintedSmoke);

    // Mantrap security door
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{0.0f, 0.0f, hallL * 0.5f + nocD + 0.02f},
        1.8f, 2.5f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true);
}

} // namespace procedural::buildings
