#include "RoomGen.h"
#include "../BuildingComponents.h"

namespace procedural::buildings {

void RoomGen::generateOfficeFloor(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = params_.width;
    float l = params_.length;
    float h = params_.ceilingHeight;
    float hw = w * 0.5f;
    float hl = l * 0.5f;

    uint32_t floorMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t wallMat = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);

    // Floor and Ceiling slabs
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, -0.1f, -hl}, Vec3{hw, 0.0f, hl}, floorMat);
    if (!params_.showInside) {
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, h, -hl}, Vec3{hw, h + 0.1f, hl}, wallMat);
    }

    // Central elevator / restroom service core
    float coreW = 3.5f;
    float coreL = 3.5f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-coreW * 0.5f, 0.0f, -coreL * 0.5f},
        Vec3{ coreW * 0.5f, h,  coreL * 0.5f},
        wallMat
    );

    // Structural reinforced concrete perimeter columns
    int colCountX = 4;
    int colCountZ = 4;
    for (int i = 0; i < colCountX; ++i) {
        float cx = -hw + 1.0f + i * ((w - 2.0f) / (colCountX - 1));
        for (int j = 0; j < colCountZ; ++j) {
            float cz = -hl + 1.0f + j * ((l - 2.0f) / (colCountZ - 1));
            // Don't place inside core
            if (std::abs(cx) < coreW * 0.6f && std::abs(cz) < coreL * 0.6f) continue;
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{cx - 0.25f, 0.0f, cz - 0.25f},
                Vec3{cx + 0.25f, h, cz + 0.25f},
                wallMat
            );
        }
    }

    // Glass Conference Room partition in North-East corner
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{hw * 0.3f, 0.0f, hl * 0.4f},
        Vec3{hw - 0.5f, h, hl * 0.42f},
        glassMat
    );
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{hw * 0.3f, 0.0f, hl * 0.4f},
        Vec3{hw * 0.32f, h, hl - 0.5f},
        glassMat
    );

    // Modular Cubicle Partitions (fabric panels and desk surfaces)
    int rows = params_.cubicleRows;
    int cols = params_.cubicleCols;
    float deskW = 1.6f;
    float deskD = 1.4f;
    float panelH = 1.35f;

    for (int r = 0; r < rows; ++r) {
        float pz = -hl + 2.5f + r * (deskD + 1.2f);
        if (pz > hl - 2.0f) break;

        for (int c = 0; c < cols; ++c) {
            float px = -hw + 2.0f + c * (deskW + 0.8f);
            if (px > -1.0f) break; // Keep left bay

            // Divider spine panel
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{px - deskW * 0.5f, 0.0f, pz},
                Vec3{px + deskW * 0.5f, panelH, pz + 0.06f},
                frameMat
            );
            // Side wing panel
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{px - deskW * 0.5f, 0.0f, pz},
                Vec3{px - deskW * 0.5f + 0.06f, panelH, pz + deskD},
                frameMat
            );
            // Desk work surface
            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{px - deskW * 0.45f, 0.72f, pz + 0.1f},
                Vec3{px + deskW * 0.45f, 0.76f, pz + deskD * 0.85f},
                wallMat
            );
        }
    }

    // Physical 3D Daylight Office Luminaires around service core & workstation aisles
    float sconceY = std::min(2.25f, h * 0.78f);
    BuildingComponents::generateWallSconceLamp(outVerts, outIndices, Vec3{0.0f, sconceY, coreL * 0.5f}, Vec3{0.0f, 0.0f, 1.0f}, true);
    BuildingComponents::generateWallSconceLamp(outVerts, outIndices, Vec3{coreW * 0.5f, sconceY, 0.0f}, Vec3{1.0f, 0.0f, 0.0f}, true);
    BuildingComponents::generateFloorStandingLamp(outVerts, outIndices, Vec3{hw * 0.55f, 0.0f, -hl * 0.35f}, std::min(1.95f, h - 0.5f), true);
    if (!params_.showInside) {
        BuildingComponents::generateCeilingPendantBulb(outVerts, outIndices, Vec3{-hw * 0.45f, h, 0.0f}, 0.50f, 0.15f, true);
        BuildingComponents::generateCeilingPendantBulb(outVerts, outIndices, Vec3{ hw * 0.45f, h, 0.0f}, 0.50f, 0.15f, true);
    }
}

} // namespace procedural::buildings
