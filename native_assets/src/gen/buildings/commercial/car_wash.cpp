#include "TowerGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>

namespace procedural::buildings {

void TowerGen::generateCarWash(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(20.0f, params_.footprintWidth);
    float d = std::max(22.0f, params_.footprintDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;
    float totalH = std::max(4.2f, params_.storyHeight);

    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t wallMat = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t hvacMat = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t glassTintedMat = static_cast<uint32_t>(MaterialSlot::GlassTinted);

    // 1. Overall Lot & Drive Apron Plinth
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 2.0f, -0.4f, -hd - 2.0f}, Vec3{hw + 2.0f, 0.0f, hd + 2.0f}, foundMat);

    // 2. Main Wash Tunnel Facility (Left Lane: X from -hw + 1.2f to -hw + 8.2f)
    float tunnelMinX = -hw + 1.2f;
    float tunnelMaxX = -hw + 8.2f;
    float tunnelW = tunnelMaxX - tunnelMinX;
    float tunnelMinZ = -hd * 0.88f;
    float tunnelMaxZ = hd * 0.88f;
    float tunnelLength = tunnelMaxZ - tunnelMinZ;
    float tunnelMidX = (tunnelMinX + tunnelMaxX) * 0.5f;

    // Reinforced concrete wash floor apron inside tunnel
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMinX, -0.05f, tunnelMinZ - 1.5f}, Vec3{tunnelMaxX, 0.05f, tunnelMaxZ + 1.5f}, foundMat);

    // Wheel guide rails along tunnel floor (twin steel rails)
    float railL = tunnelMinX + tunnelW * 0.28f;
    float railR = tunnelMinX + tunnelW * 0.72f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{railL - 0.08f, 0.05f, tunnelMinZ - 1.0f}, Vec3{railL + 0.08f, 0.22f, tunnelMaxZ + 1.0f}, frameMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{railR - 0.08f, 0.05f, tunnelMinZ - 1.0f}, Vec3{railR + 0.08f, 0.22f, tunnelMaxZ + 1.0f}, frameMat);

    // Tunnel outer left wall (solid corrugated industrial wall)
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMinX - 0.25f, 0.0f, tunnelMinZ}, Vec3{tunnelMinX, totalH, tunnelMaxZ}, wallMat);

    // Tunnel roof deck with parapet & industrial exhaust vents
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMinX - 0.35f, totalH, tunnelMinZ - 0.35f}, Vec3{tunnelMaxX + 0.35f, totalH + 0.45f, tunnelMaxZ + 0.35f}, roofMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMinX + 1.0f, totalH + 0.45f, tunnelMinZ + 2.0f}, Vec3{tunnelMinX + 2.5f, totalH + 1.2f, tunnelMinZ + 4.5f}, hvacMat);

    // Tunnel right wall (Observation Viewing Gallery with panoramic glass panels)
    int glassPillars = 5;
    float segZ = tunnelLength / static_cast<float>(glassPillars);
    for (int p = 0; p < glassPillars; ++p) {
        float z0 = tunnelMinZ + p * segZ;
        float z1 = z0 + segZ;
        // Vertical structural steel mullion
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMaxX - 0.15f, 0.0f, z0 - 0.1f}, Vec3{tunnelMaxX + 0.15f, totalH, z0 + 0.1f}, frameMat);
        // Concrete knee-wall
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMaxX - 0.12f, 0.0f, z0}, Vec3{tunnelMaxX + 0.12f, 0.65f, z1}, wallMat);
        // Header beam
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMaxX - 0.12f, totalH - 0.55f, z0}, Vec3{tunnelMaxX + 0.12f, totalH, z1}, wallMat);
        // Clear observation glass pane
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMaxX - 0.03f, 0.65f, z0 + 0.05f}, Vec3{tunnelMaxX + 0.03f, totalH - 0.55f, z1 - 0.05f}, glassMat);
    }
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMaxX - 0.15f, 0.0f, tunnelMaxZ - 0.1f}, Vec3{tunnelMaxX + 0.15f, totalH, tunnelMaxZ + 0.1f}, frameMat);

    // Entrance Portal (at tunnelMinZ):
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMinX - 0.3f, totalH - 0.85f, tunnelMinZ - 0.1f}, Vec3{tunnelMaxX + 0.3f, totalH, tunnelMinZ + 0.2f}, frameMat);
    // Clearance warning hazard bar
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMinX + 0.2f, totalH - 0.7f, tunnelMinZ - 0.15f}, Vec3{tunnelMaxX - 0.2f, totalH - 0.5f, tunnelMinZ - 0.11f}, wallMat);
    // Traffic signal indicator (Red/Green entry signal)
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMaxX + 0.2f, 2.2f, tunnelMinZ - 0.2f}, Vec3{tunnelMaxX + 0.55f, 3.4f, tunnelMinZ + 0.1f}, frameMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMaxX + 0.25f, 2.85f, tunnelMinZ - 0.25f}, Vec3{tunnelMaxX + 0.50f, 3.25f, tunnelMinZ - 0.21f}, glassTintedMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMaxX + 0.25f, 2.35f, tunnelMinZ - 0.25f}, Vec3{tunnelMaxX + 0.50f, 2.75f, tunnelMinZ - 0.21f}, glassTintedMat);

    // Exit Portal (at tunnelMaxZ):
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMinX - 0.3f, totalH - 0.85f, tunnelMaxZ - 0.2f}, Vec3{tunnelMaxX + 0.3f, totalH, tunnelMaxZ + 0.1f}, frameMat);

    // 3. Tunnel Interior Washing Equipment
    // Stage 1: Pre-Soak Spray Arch (at 20% length)
    float soakZ = tunnelMinZ + tunnelLength * 0.20f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMinX + 0.3f, 0.0f, soakZ - 0.15f}, Vec3{tunnelMinX + 0.55f, totalH - 0.4f, soakZ + 0.15f}, frameMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMaxX - 0.55f, 0.0f, soakZ - 0.15f}, Vec3{tunnelMaxX - 0.3f, totalH - 0.4f, soakZ + 0.15f}, frameMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMinX + 0.3f, totalH - 0.65f, soakZ - 0.15f}, Vec3{tunnelMaxX - 0.3f, totalH - 0.4f, soakZ + 0.15f}, frameMat);

    // Stage 2: Twin Giant Vertical Rotating Foam Wash Brushes (at 42% length)
    float brushZ = tunnelMinZ + tunnelLength * 0.42f;
    float brushLeftX = tunnelMinX + 1.25f;
    float brushRightX = tunnelMaxX - 1.25f;
    float brushH = 2.7f;
    float brushR = 0.65f;

    // Left brush cylinder & steel spindle
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{brushLeftX - brushR, 0.2f, brushZ - brushR}, Vec3{brushLeftX + brushR, brushH, brushZ + brushR}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{brushLeftX - 0.08f, 0.0f, brushZ - 0.08f}, Vec3{brushLeftX + 0.08f, brushH + 0.35f, brushZ + 0.08f}, frameMat);

    // Right brush cylinder & steel spindle
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{brushRightX - brushR, 0.2f, brushZ - brushR}, Vec3{brushRightX + brushR, brushH, brushZ + brushR}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{brushRightX - 0.08f, 0.0f, brushZ - 0.08f}, Vec3{brushRightX + 0.08f, brushH + 0.35f, brushZ + 0.08f}, frameMat);

    // Stage 3: Overhead Horizontal Roller Brush (at 58% length)
    float roofBrushZ = tunnelMinZ + tunnelLength * 0.58f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMinX + 0.6f, 2.1f, roofBrushZ - 0.45f}, Vec3{tunnelMaxX - 0.6f, 3.0f, roofBrushZ + 0.45f}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMinX + 0.3f, 2.45f, roofBrushZ - 0.08f}, Vec3{tunnelMaxX - 0.3f, 2.65f, roofBrushZ + 0.08f}, frameMat);

    // Stage 4: High-Velocity Air Dryer Blower Hood Arch (at 78% length)
    float dryerZ = tunnelMinZ + tunnelLength * 0.78f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMinX + 0.25f, 0.0f, dryerZ - 0.2f}, Vec3{tunnelMinX + 0.5f, totalH - 0.3f, dryerZ + 0.2f}, frameMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMaxX - 0.5f, 0.0f, dryerZ - 0.2f}, Vec3{tunnelMaxX - 0.25f, totalH - 0.3f, dryerZ + 0.2f}, frameMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tunnelMinX + 0.4f, totalH - 1.2f, dryerZ - 0.4f}, Vec3{tunnelMaxX - 0.4f, totalH - 0.5f, dryerZ + 0.4f}, wallMat);

    // Blower nozzles
    for (int b = -1; b <= 1; ++b) {
        float bx = tunnelMidX + b * (tunnelW * 0.26f);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{bx - 0.3f, totalH - 1.45f, dryerZ - 0.25f}, Vec3{bx + 0.3f, totalH - 1.15f, dryerZ + 0.25f}, frameMat);
    }

    // 4. Equipment, Chemical & Water Reclamation Pump Room
    float pumpMinX = tunnelMaxX;
    float pumpMaxX = tunnelMaxX + 4.8f;
    float pumpMinZ = -hd * 0.4f;
    float pumpMaxZ = hd * 0.4f;
    float pumpH = 3.4f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{pumpMinX, 0.0f, pumpMinZ}, Vec3{pumpMaxX, pumpH, pumpMaxZ}, wallMat);

    // Service door & ventilation grilles
    BuildingComponents::generateDoor(outVerts, outIndices, Vec3{pumpMaxX, 0.0f, pumpMinZ + 1.4f}, 1.2f, 2.3f, Vec3{1, 0, 0}, Vec3{0, 0, 1}, DoorType::IndustrialRollUp, false);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{pumpMaxX - 0.02f, 1.2f, pumpMaxZ - 2.0f}, Vec3{pumpMaxX + 0.05f, 2.4f, pumpMaxZ - 0.6f}, frameMat);

    // Rooftop chemical exhaust pipes & condenser
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{pumpMinX + 1.0f, pumpH, pumpMinZ + 1.0f}, Vec3{pumpMinX + 2.4f, pumpH + 0.8f, pumpMinZ + 2.4f}, hvacMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{pumpMinX + 3.0f, pumpH, pumpMaxZ - 1.8f}, Vec3{pumpMinX + 3.4f, pumpH + 1.4f, pumpMaxZ - 1.4f}, frameMat);

    // 5. Automated Driver Payment Entry Kiosk
    float kioskX = tunnelMidX + 1.6f;
    float kioskZ = tunnelMinZ - 3.0f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{kioskX - 0.35f, 0.0f, kioskZ - 0.35f}, Vec3{kioskX + 0.35f, 1.5f, kioskZ + 0.35f}, frameMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{kioskX - 0.30f, 0.9f, kioskZ + 0.35f}, Vec3{kioskX + 0.30f, 1.4f, kioskZ + 0.40f}, glassTintedMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{kioskX - 0.45f, 1.5f, kioskZ - 0.45f}, Vec3{kioskX + 0.45f, 1.6f, kioskZ + 0.55f}, roofMat);

    // 6. Outdoor Self-Service Vacuum Island (Right side of lot: X from hw * 0.40f to hw * 0.92f)
    float vacMinX = hw * 0.40f;
    float vacMaxX = hw * 0.92f;
    float vacMinZ = -hd * 0.65f;
    float vacMaxZ = hd * 0.65f;
    float vacMidX = (vacMinX + vacMaxX) * 0.5f;

    // Raised concrete vacuum island curb
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{vacMinX, 0.0f, vacMinZ}, Vec3{vacMaxX, 0.22f, vacMaxZ}, foundMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{vacMinX - 0.04f, 0.18f, vacMinZ - 0.04f}, Vec3{vacMaxX + 0.04f, 0.24f, vacMaxZ + 0.04f}, frameMat);

    // Overhead tubular arch frame & boom arms
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{vacMidX - 0.15f, 0.22f, vacMinZ + 1.0f}, Vec3{vacMidX + 0.15f, 3.8f, vacMinZ + 1.3f}, frameMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{vacMidX - 0.15f, 0.22f, vacMaxZ - 1.3f}, Vec3{vacMidX + 0.15f, 3.8f, vacMaxZ - 1.0f}, frameMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{vacMidX - 0.15f, 3.6f, vacMinZ + 1.0f}, Vec3{vacMidX + 0.15f, 3.8f, vacMaxZ - 1.0f}, frameMat);

    // Twin vacuum turbine motor canisters & suction hoses
    float vacCanisterZs[2] = {vacMinZ + (vacMaxZ - vacMinZ) * 0.35f, vacMinZ + (vacMaxZ - vacMinZ) * 0.65f};
    for (float vcz : vacCanisterZs) {
        // Motor cylinder
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{vacMidX - 0.32f, 0.22f, vcz - 0.32f}, Vec3{vacMidX + 0.32f, 1.6f, vcz + 0.32f}, frameMat);
        // Dome top
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{vacMidX - 0.35f, 1.6f, vcz - 0.35f}, Vec3{vacMidX + 0.35f, 1.8f, vcz + 0.35f}, wallMat);
        // Suction hoses hanging down
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{vacMidX - 0.45f, 0.4f, vcz - 0.1f}, Vec3{vacMidX - 0.38f, 1.5f, vcz + 0.1f}, frameMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{vacMidX + 0.38f, 0.4f, vcz - 0.1f}, Vec3{vacMidX + 0.45f, 1.5f, vcz + 0.1f}, frameMat);
    }
}

} // namespace procedural::buildings
