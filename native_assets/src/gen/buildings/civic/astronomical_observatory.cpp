#include "CivicGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {
constexpr float kObsPi = 3.14159265358979323846f;
}

void CivicGen::generateObservatory(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(24.0f, params_.mainWidth);
    float d = std::max(18.0f, params_.mainDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t limestone   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t concrete    = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t gunmetal    = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t silverAlum  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t frameMat    = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t roofMat     = static_cast<uint32_t>(MaterialSlot::RoofSurface);

    // 1. Elevated Hilltop Stone Plaza Platform
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.5f, -0.45f, -hd - 2.0f},
        Vec3{ hw + 2.5f,  0.00f,  hd + 3.0f},
        foundMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.5f, 0.00f, -hd - 0.5f},
        Vec3{ hw + 0.5f, 0.55f,  hd + 0.8f},
        limestone);

    // 2. Research Wing & Planetarium Gallery (Left Wing)
    float wingX0 = -hw + 0.5f;
    float wingX1 = -1.5f;
    float wingH  = 5.8f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{wingX0, 0.55f, -hd * 0.55f},
        Vec3{wingX1, wingH,  hd * 0.55f},
        concrete);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{wingX0 - 0.15f, wingH, -hd * 0.55f - 0.15f},
        Vec3{wingX1 + 0.15f, wingH + 0.45f, hd * 0.55f + 0.15f},
        gunmetal);
    // Ribbon daylight windows on research wing
    BuildingComponents::generateWindow(outVerts, outIndices,
        Vec3{(wingX0 + wingX1) * 0.5f, 3.4f, hd * 0.55f},
        (wingX1 - wingX0) * 0.72f, 1.5f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        WindowType::RibbonStrip, GlassType::SolarTintedSmoke);
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{(wingX0 + wingX1) * 0.5f, 0.55f, hd * 0.55f},
        2.4f, 2.6f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true);

    // Secondary Small Solar Telescope Dome atop Left Wing
    float subX = wingX0 + 3.5f;
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{subX, wingH + 0.45f, 0.0f},
        Vec3{subX, wingH + 2.0f,  0.0f},
        2.3f, 2.3f, 14, limestone);
    int subSegs = 14;
    for (int i = 0; i < subSegs; ++i) {
        float a0 = (2.0f * kObsPi * i) / subSegs;
        float a1 = (2.0f * kObsPi * (i + 1)) / subSegs;
        for (int r = 0; r < 3; ++r) {
            float p0 = (0.5f * kObsPi * r) / 3.0f;
            float p1 = (0.5f * kObsPi * (r + 1)) / 3.0f;
            float r0 = 2.35f * std::cos(p0), r1 = 2.35f * std::cos(p1);
            float y0 = wingH + 2.0f + 2.3f * std::sin(p0);
            float y1 = wingH + 2.0f + 2.3f * std::sin(p1);
            Vec3 n{std::cos((a0 + a1) * 0.5f) * std::cos((p0 + p1) * 0.5f), std::sin((p0 + p1) * 0.5f), std::sin((a0 + a1) * 0.5f) * std::cos((p0 + p1) * 0.5f)};
            BuildingComponents::appendQuad(outVerts, outIndices,
                Vec3{subX + std::cos(a0) * r0, y0, std::sin(a0) * r0},
                Vec3{subX + std::cos(a1) * r0, y0, std::sin(a1) * r0},
                Vec3{subX + std::cos(a1) * r1, y1, std::sin(a1) * r1},
                Vec3{subX + std::cos(a0) * r1, y1, std::sin(a0) * r1},
                n, silverAlum);
        }
    }

    // 3. Monumental Main Telescope Rotunda Drum & Hemispherical Rotating Dome (Right-Center)
    float domX = 4.5f;
    float domR = std::min(7.2f, hd * 0.85f);
    float drumBaseY = 0.55f;
    float drumTopY  = 9.2f;

    // Cylindrical rotunda drum + architectural vertical pilasters
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{domX, drumBaseY, 0.0f},
        Vec3{domX, drumTopY,  0.0f},
        domR, domR, 20, limestone);
    // Upper rotating steel ring track collar
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{domX, drumTopY, 0.0f},
        Vec3{domX, drumTopY + 0.85f, 0.0f},
        domR + 0.35f, domR + 0.35f, 20, blackAlum);

    // Exterior observation catwalk balcony ring around rotunda drum
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{domX, drumTopY - 0.55f, 0.0f},
        Vec3{domX, drumTopY - 0.30f, 0.0f},
        domR + 1.15f, domR + 1.15f, 20, roofMat);

    int segs = 20;
    for (int i = 0; i < segs; ++i) {
        float ang = (2.0f * kObsPi * i) / segs;
        float px = domX + std::cos(ang) * (domR + 0.1f);
        float pz = std::sin(ang) * (domR + 0.1f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 0.22f, drumBaseY, pz - 0.22f},
            Vec3{px + 0.22f, drumTopY - 0.55f, pz + 0.22f},
            concrete);
        // Perimeter observation balcony safety posts
        float bx = domX + std::cos(ang) * (domR + 1.05f);
        float bz = std::sin(ang) * (domR + 1.05f);
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{bx, drumTopY - 0.30f, bz},
            Vec3{bx, drumTopY + 0.70f, bz},
            0.05f, 0.05f, 6, silverAlum);
    }

    // Segmented Aluminum Observatory Dome with Open Viewing Aperture Slot facing +Z!
    float domeStartY = drumTopY + 0.85f;
    float domeRadius = domR + 0.15f;
    int rings = 5;
    for (int i = 0; i < segs; ++i) {
        float a0 = (2.0f * kObsPi * i) / segs;
        float a1 = (2.0f * kObsPi * (i + 1)) / segs;
        float midA = (a0 + a1) * 0.5f;
        // Leave aperture slit open near +Z (angle ~ pi/2)
        bool isShutterSlot = (std::abs(midA - 0.5f * kObsPi) < 0.26f);

        for (int r = 0; r < rings; ++r) {
            if (isShutterSlot && r >= 1 && r <= 3) continue; // Open telescope slit!
            float p0 = (0.5f * kObsPi * r) / rings;
            float p1 = (0.5f * kObsPi * (r + 1)) / rings;
            float rr0 = domeRadius * std::cos(p0);
            float rr1 = domeRadius * std::cos(p1);
            float yy0 = domeStartY + domeRadius * std::sin(p0);
            float yy1 = domeStartY + domeRadius * std::sin(p1);
            Vec3 norm{
                std::cos(midA) * std::cos((p0 + p1) * 0.5f),
                std::sin((p0 + p1) * 0.5f),
                std::sin(midA) * std::cos((p0 + p1) * 0.5f)
            };
            BuildingComponents::appendQuad(outVerts, outIndices,
                Vec3{domX + std::cos(a0) * rr0, yy0, std::sin(a0) * rr0},
                Vec3{domX + std::cos(a1) * rr0, yy0, std::sin(a1) * rr0},
                Vec3{domX + std::cos(a1) * rr1, yy1, std::sin(a1) * rr1},
                Vec3{domX + std::cos(a0) * rr1, yy1, std::sin(a0) * rr1},
                norm, isShutterSlot ? blackAlum : silverAlum);
        }
    }

    // Giant Astronomical Telescope Optical Tube emerging upward at 42° out of the shutter slot!
    Vec3 scopePivot{domX, domeStartY + domeRadius * 0.28f, 0.0f};
    Vec3 scopeTip{domX, domeStartY + domeRadius * 0.92f, domeRadius * 0.85f};
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        scopePivot, scopeTip, 1.05f, 0.92f, 14, blackAlum);
    // Telescope Dew Shield Collar & Glowing Objective Lens
    Vec3 lensEnd{domX, scopeTip.y + 0.65f, scopeTip.z + 0.72f};
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        scopeTip, lensEnd, 0.98f, 0.98f, 14, frameMat);
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        lensEnd, Vec3{domX, lensEnd.y + 0.06f, lensEnd.z + 0.06f},
        0.84f, 0.84f, 14, glassTinted);
}

} // namespace procedural::buildings
