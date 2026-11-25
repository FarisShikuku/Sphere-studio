#include "CivicGen.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void CivicGen::generateModernSanctuary(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(24.0f, params_.mainWidth);
    float d = std::max(26.0f, params_.mainDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t timberMat   = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t pergolaMat  = static_cast<uint32_t>(MaterialSlot::PergolaWood);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t concrete    = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t roofMat     = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t stainedMat  = static_cast<uint32_t>(MaterialSlot::GlassStained);
    uint32_t poolMat     = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t champAlum   = static_cast<uint32_t>(MaterialSlot::WallChampagneAluminum);

    // 1. Stone Piazza Plinth & Reflecting Water Basin
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, -0.35f, -hd - 2.0f},
        Vec3{ hw + 2.0f,  0.00f,  hd + 4.5f},
        foundMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.00f, -hd},
        Vec3{ hw, 0.45f,  hd + 1.5f},
        concrete);
    // Flanking Emerald Reflecting Pool in Forecourt
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.2f, 0.45f, hd - 1.5f},
        Vec3{-4.2f,      0.56f, hd + 1.1f},
        poolMat);

    // 2. Soaring Sculptural A-Frame / Swept Timber Folded-Roof Sanctuary Hall
    float aHW = std::min(10.5f, hw * 0.72f);
    float apexFrontH = 16.5f;
    float apexRearH  = 11.2f;
    float baseZ0 = -hd + 1.5f;
    float baseZ1 =  hd - 2.5f;

    // Heavy Timber Sloped A-Frame Roof Wings (thickened quads on left and right)
    // Outer Left slope
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{-aHW - 1.2f, 0.45f, baseZ1},
        Vec3{-aHW - 1.2f, 0.45f, baseZ0},
        Vec3{0.0f, apexRearH,  baseZ0},
        Vec3{0.0f, apexFrontH, baseZ1},
        Vec3{-0.82f, 0.56f, 0.0f}, roofMat);
    // Inner Left timber ceiling soffit
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{0.0f, apexFrontH - 0.55f, baseZ1},
        Vec3{0.0f, apexRearH  - 0.55f, baseZ0},
        Vec3{-aHW - 0.5f, 0.45f, baseZ0},
        Vec3{-aHW - 0.5f, 0.45f, baseZ1},
        Vec3{0.82f, -0.56f, 0.0f}, pergolaMat);

    // Outer Right slope
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{aHW + 1.2f, 0.45f, baseZ0},
        Vec3{aHW + 1.2f, 0.45f, baseZ1},
        Vec3{0.0f, apexFrontH, baseZ1},
        Vec3{0.0f, apexRearH,  baseZ0},
        Vec3{0.82f, 0.56f, 0.0f}, roofMat);
    // Inner Right timber ceiling soffit
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{aHW + 0.5f, 0.45f, baseZ1},
        Vec3{aHW + 0.5f, 0.45f, baseZ0},
        Vec3{0.0f, apexRearH  - 0.55f, baseZ0},
        Vec3{0.0f, apexFrontH - 0.55f, baseZ1},
        Vec3{-0.82f, -0.56f, 0.0f}, pergolaMat);

    // Exposed Glulam Timber Portal Ribs stepping down exterior A-Frame
    int ribs = 5;
    for (int r = 0; r < ribs; ++r) {
        float t = r / static_cast<float>(ribs - 1);
        float rz = baseZ0 + t * (baseZ1 - baseZ0);
        float rh = apexRearH + t * (apexFrontH - apexRearH);
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{-aHW - 1.3f, 0.45f, rz},
            Vec3{0.0f, rh + 0.15f, rz},
            0.26f, 0.24f, 6, timberMat);
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{ aHW + 1.3f, 0.45f, rz},
            Vec3{0.0f, rh + 0.15f, rz},
            0.26f, 0.24f, 6, timberMat);
    }

    // 3. Dramatic Full-Height Front & Rear Stained + Tinted Glass Curtain Walls
    // Triangular Glazed Front Curtain Wall with Stained Glass Vertical Accent Strip
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{-aHW + 0.5f, 0.45f, baseZ1 - 0.15f},
        Vec3{ aHW - 0.5f, 0.45f, baseZ1 - 0.15f},
        Vec3{0.5f, apexFrontH - 0.8f, baseZ1 - 0.15f},
        Vec3{-0.5f, apexFrontH - 0.8f, baseZ1 - 0.15f},
        Vec3{0, 0, 1}, glassTinted);
    // Central vertical stained glass art strip rising up to peak
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-1.3f, 3.5f, baseZ1 - 0.20f},
        Vec3{ 1.3f, apexFrontH - 2.2f, baseZ1 + 0.05f},
        stainedMat);
    // Back choir wall
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{ aHW - 0.5f, 0.45f, baseZ0 + 0.15f},
        Vec3{-aHW + 0.5f, 0.45f, baseZ0 + 0.15f},
        Vec3{-0.5f, apexRearH - 0.8f, baseZ0 + 0.15f},
        Vec3{0.5f, apexRearH - 0.8f, baseZ0 + 0.15f},
        Vec3{0, 0, -1}, timberMat);

    // Architectural Narthex Entrance Canopy & Double Glass Doors
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-3.8f, 3.6f, baseZ1 - 0.2f},
        Vec3{ 3.8f, 3.95f, baseZ1 + 2.4f},
        blackAlum);
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{0.0f, 0.45f, baseZ1},
        2.8f, 3.1f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, false);

    // 4. Detached Minimalist Bell Campanile Pylon & Champagne Cross (Front-Right Forecourt)
    float pylonX = hw - 3.2f;
    float pylonZ = hd - 1.2f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{pylonX - 0.85f, 0.45f, pylonZ - 0.85f},
        Vec3{pylonX + 0.85f, 17.5f, pylonZ + 0.85f},
        concrete);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{pylonX - 0.18f, 11.0f, pylonZ + 0.85f},
        Vec3{pylonX + 0.18f, 19.5f, pylonZ + 1.15f},
        champAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{pylonX - 2.2f, 15.2f, pylonZ + 0.85f},
        Vec3{pylonX + 2.2f, 15.6f, pylonZ + 1.15f},
        champAlum);
}

} // namespace procedural::buildings
