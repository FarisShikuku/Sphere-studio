#include "WarehouseGen.h"
#include "../BuildingComponents.h"
#include <cmath>
#include <algorithm>

namespace procedural::buildings {

void WarehouseGen::generateFactory(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = params_.length;
    float d = params_.width;
    float h = std::max(6.0f, params_.height);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t wallMat = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t hvacMat = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t brickMat = static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t stoneMat = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t stuccoMat = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t darkMetalMat = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t silverMetalMat = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t accentMat = static_cast<uint32_t>(MaterialSlot::WallSpandrelGlass);

    // 1. Concrete Industrial Foundation & Machine Yard Pad
    float yardMinX = -hw - 4.0f;
    float yardMaxX = hw + 4.0f;
    float yardMinZ = -hd - 4.0f;
    float yardMaxZ = hd + 3.0f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{yardMinX, -0.4f, yardMinZ}, Vec3{yardMaxX, 0.0f, yardMaxZ}, foundMat);

    // 2. Fired Terracotta Brick Masonry Plinth (Ground to 2.6m)
    float plinthH = 2.6f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 0.15f, 0.0f, -hd - 0.15f}, Vec3{hw + 0.15f, plinthH, hd + 0.15f}, brickMat);
    // Limestone Water Table / Belt Course Molding
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 0.22f, plinthH - 0.12f, -hd - 0.22f}, Vec3{hw + 0.22f, plinthH + 0.08f, hd + 0.22f}, stoneMat);

    // 3. Upper Corrugated Metal Factory Envelope
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw, plinthH, -hd}, Vec3{hw, h, hd}, wallMat);

    // 4. Exposed Structural Steel I-Beam Buttress Pilasters along Facades
    int numBayPillars = 6;
    for (int i = 0; i <= numBayPillars; ++i) {
        float fx = -hw + i * ((hw * 2.0f) / numBayPillars);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{fx - 0.22f, 0.0f, hd - 0.05f}, Vec3{fx + 0.22f, h + 0.4f, hd + 0.28f}, darkMetalMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{fx - 0.22f, 0.0f, -hd - 0.28f}, Vec3{fx + 0.22f, h + 0.4f, -hd + 0.05f}, darkMetalMat);
    }

    // 5. Industrial Multi-Pane Factory Grid Windows
    for (int i = 0; i < numBayPillars; ++i) {
        float bayCenterX = -hw + (i + 0.5f) * ((hw * 2.0f) / numBayPillars);
        float winW = 2.4f;
        float winH = 2.8f;
        float winY = 1.2f;

        // Front windows (skip center bay for roll-up entrance)
        if (i != numBayPillars / 2) {
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{bayCenterX - winW * 0.5f, winY, hd + 0.08f}, Vec3{bayCenterX + winW * 0.5f, winY + winH, hd + 0.18f}, glassMat);
            for (int gx = 1; gx <= 2; ++gx) {
                float mx = bayCenterX - winW * 0.5f + gx * (winW / 3.0f);
                BuildingComponents::appendBox(outVerts, outIndices, Vec3{mx - 0.03f, winY, hd + 0.07f}, Vec3{mx + 0.03f, winY + winH, hd + 0.20f}, darkMetalMat);
            }
            for (int gy = 1; gy <= 3; ++gy) {
                float my = winY + gy * (winH / 4.0f);
                BuildingComponents::appendBox(outVerts, outIndices, Vec3{bayCenterX - winW * 0.5f, my - 0.03f, hd + 0.07f}, Vec3{bayCenterX + winW * 0.5f, my + 0.03f, hd + 0.20f}, darkMetalMat);
            }
        }

        // Back wall windows
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{bayCenterX - winW * 0.5f, winY, -hd - 0.18f}, Vec3{bayCenterX + winW * 0.5f, winY + winH, -hd - 0.08f}, glassMat);
        for (int gx = 1; gx <= 2; ++gx) {
            float mx = bayCenterX - winW * 0.5f + gx * (winW / 3.0f);
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{mx - 0.03f, winY, -hd - 0.20f}, Vec3{mx + 0.03f, winY + winH, -hd - 0.07f}, darkMetalMat);
        }
        for (int gy = 1; gy <= 3; ++gy) {
            float my = winY + gy * (winH / 4.0f);
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{bayCenterX - winW * 0.5f, my - 0.03f, -hd - 0.20f}, Vec3{bayCenterX + winW * 0.5f, my + 0.03f, -hd - 0.07f}, darkMetalMat);
        }
    }

    // 6. Factory Equipment Roll-up Bay & Personnel Shift Portal
    float centerBayX = 0.0f;
    float gateW = 4.2f;
    float gateH = 4.5f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{centerBayX - gateW * 0.5f, 0.0f, hd - 0.05f}, Vec3{centerBayX + gateW * 0.5f, gateH, hd + 0.15f}, darkMetalMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{centerBayX - gateW * 0.5f + 0.2f, 0.2f, hd + 0.10f}, Vec3{centerBayX + gateW * 0.5f - 0.2f, gateH - 0.2f, hd + 0.18f}, silverMetalMat);
    // Yellow hazard warning threshold
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{centerBayX - gateW * 0.5f, 0.0f, hd + 0.15f}, Vec3{centerBayX + gateW * 0.5f, 0.08f, hd + 0.65f}, frameMat);

    // Personnel shift entry on left side
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 0.15f, 0.0f, -hd * 0.3f}, Vec3{-hw + 0.05f, 2.4f, -hd * 0.3f + 1.4f}, darkMetalMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw - 1.2f, 2.5f, -hd * 0.3f - 0.2f}, Vec3{-hw, 2.65f, -hd * 0.3f + 1.6f}, accentMat);

    // 7. Iconic Sawtooth North-Light Roof (4 bays)
    int teeth = 4;
    float toothW = (hw * 2.0f) / teeth;
    float toothH = 2.8f;

    for (int t = 0; t < teeth; ++t) {
        float xStart = -hw + t * toothW;
        float xPeak = xStart + toothW * 0.78f;
        float xEnd = xStart + toothW;

        // Sloped roof facet
        BuildingComponents::appendQuad(
            outVerts, outIndices,
            Vec3{xStart, h, -hd}, Vec3{xPeak, h + toothH, -hd},
            Vec3{xPeak, h + toothH, hd}, Vec3{xStart, h, hd},
            Vec3{-0.35f, 1.0f, 0.0f}.normalized(), roofMat
        );

        // Vertical glazed skylight facet (facing north for diffuse daylight)
        BuildingComponents::appendQuad(
            outVerts, outIndices,
            Vec3{xPeak, h + toothH, -hd}, Vec3{xEnd, h, -hd},
            Vec3{xEnd, h, hd}, Vec3{xPeak, h + toothH, hd},
            Vec3{1.0f, 0.0f, 0.0f}, glassMat
        );

        // Triangular gable infills
        Vec3 gFront0{xStart, h, hd}; Vec3 gFront1{xPeak, h + toothH, hd}; Vec3 gFront2{xEnd, h, hd};
        BuildingComponents::appendQuad(outVerts, outIndices, gFront0, gFront1, gFront1, gFront2, Vec3{0, 0, 1}, wallMat);

        Vec3 gBack0{xEnd, h, -hd}; Vec3 gBack1{xPeak, h + toothH, -hd}; Vec3 gBack2{xStart, h, -hd};
        BuildingComponents::appendQuad(outVerts, outIndices, gBack0, gBack1, gBack1, gBack2, Vec3{0, 0, -1}, wallMat);
    }

    // 8. Authentic Cylindrical Tapered Smokestack with Aviation Hazard Banding
    float stackX = hw - 2.5f;
    float stackZ = -hd - 1.8f;
    float stackBaseR = 1.25f;
    float stackTopR = 0.78f;
    float stackH = h + 9.5f;
    int stackSegs = 16;
    float pi2 = 6.28318530718f;

    // Concrete plinth foundation
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{stackX - 1.8f, 0.0f, stackZ - 1.8f}, Vec3{stackX + 1.8f, 0.6f, stackZ + 1.8f}, foundMat);

    // Alternating Red & White Aviation Safety Bands
    int numBands = 7;
    float bandH = (stackH - 0.6f) / numBands;
    for (int b = 0; b < numBands; ++b) {
        float y0 = 0.6f + b * bandH;
        float y1 = y0 + bandH;
        float r0 = stackBaseR + (stackTopR - stackBaseR) * (static_cast<float>(b) / numBands);
        float r1 = stackBaseR + (stackTopR - stackBaseR) * (static_cast<float>(b + 1) / numBands);
        uint32_t bandMat = (b % 2 == 0) ? brickMat : stuccoMat;

        for (int i = 0; i < stackSegs; ++i) {
            float a0 = (static_cast<float>(i) / stackSegs) * pi2;
            float a1 = (static_cast<float>(i + 1) / stackSegs) * pi2;
            Vec3 p0{stackX + std::cos(a0) * r0, y0, stackZ + std::sin(a0) * r0};
            Vec3 p1{stackX + std::cos(a1) * r0, y0, stackZ + std::sin(a1) * r0};
            Vec3 p2{stackX + std::cos(a1) * r1, y1, stackZ + std::sin(a1) * r1};
            Vec3 p3{stackX + std::cos(a0) * r1, y1, stackZ + std::sin(a0) * r1};
            float aMid = (a0 + a1) * 0.5f;
            Vec3 norm{std::cos(aMid), 0.0f, std::sin(aMid)};
            BuildingComponents::appendQuad(outVerts, outIndices, p0, p1, p2, p3, norm, bandMat);
        }
    }

    // Flanged exhaust emission crown rim
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{stackX - stackTopR - 0.15f, stackH - 0.1f, stackZ - stackTopR - 0.15f}, Vec3{stackX + stackTopR + 0.15f, stackH + 0.25f, stackZ + stackTopR + 0.15f}, darkMetalMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{stackX - stackTopR + 0.1f, stackH + 0.20f, stackZ - stackTopR + 0.1f}, Vec3{stackX + stackTopR - 0.1f, stackH + 0.26f, stackZ + stackTopR - 0.1f}, darkMetalMat);

    // Caged ladder on stack
    float slX = stackX + stackTopR + 0.05f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{slX, 0.6f, stackZ - 0.2f}, Vec3{slX + 0.08f, stackH - 0.5f, stackZ + 0.2f}, silverMetalMat);

    // 9. Outdoor Industrial Mechanical Infrastructure Yard
    // A) Elevated Pipe Gantry Bridge
    float gantryZ = -hd - 1.2f;
    float gantryY = h * 0.65f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw * 0.4f, gantryY - 0.12f, gantryZ - 0.25f}, Vec3{hw * 0.5f, gantryY + 0.12f, gantryZ + 0.25f}, darkMetalMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw * 0.4f, gantryY + 0.15f, gantryZ - 0.15f}, Vec3{hw * 0.5f, gantryY + 0.45f, gantryZ - 0.02f}, silverMetalMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-hw * 0.4f, gantryY + 0.15f, gantryZ + 0.02f}, Vec3{hw * 0.5f, gantryY + 0.45f, gantryZ + 0.15f}, frameMat);
    for (float stX : {-hw * 0.4f, 0.0f, hw * 0.45f}) {
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{stX - 0.12f, 0.0f, gantryZ - 0.12f}, Vec3{stX + 0.12f, gantryY, gantryZ + 0.12f}, darkMetalMat);
    }

    // B) Cyclone Dust Collector & Hopper
    float cycX = -hw - 1.8f;
    float cycZ = 0.0f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{cycX - 1.1f, 2.2f, cycZ - 1.1f}, Vec3{cycX + 1.1f, 5.0f, cycZ + 1.1f}, hvacMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{cycX - 0.5f, 1.2f, cycZ - 0.5f}, Vec3{cycX + 0.5f, 2.2f, cycZ + 0.5f}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{cycX - 0.4f, 0.0f, cycZ - 0.4f}, Vec3{cycX + 0.4f, 1.2f, cycZ + 0.4f}, darkMetalMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{cycX, 4.2f, cycZ - 0.2f}, Vec3{-hw, 4.6f, cycZ + 0.2f}, darkMetalMat);

    // C) Horizontal Chemical Pressure Vessel Tank
    float tankX = -hw * 0.4f;
    float tankZ = -hd - 2.8f;
    float tankLen = 4.5f;
    float tankR = 1.1f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tankX - 0.3f, 0.0f, tankZ - 0.9f}, Vec3{tankX + 0.3f, 0.8f, tankZ + 0.9f}, foundMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tankX + tankLen - 0.3f, 0.0f, tankZ - 0.9f}, Vec3{tankX + tankLen + 0.3f, 0.8f, tankZ + 0.9f}, foundMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tankX, 0.5f, tankZ - tankR}, Vec3{tankX + tankLen, 0.5f + tankR * 2.0f, tankZ + tankR}, silverMetalMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{tankX + tankLen * 0.5f - 0.35f, 0.5f + tankR * 2.0f, tankZ - 0.35f}, Vec3{tankX + tankLen * 0.5f + 0.35f, 0.5f + tankR * 2.0f + 0.45f, tankZ + 0.35f}, accentMat);

    // D) High-Voltage Electrical Substation Transformer Cage
    float subX = hw * 0.4f;
    float subZ = -hd - 2.8f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{subX - 1.5f, 0.0f, subZ - 1.5f}, Vec3{subX + 1.5f, 0.25f, subZ + 1.5f}, foundMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{subX - 0.9f, 0.25f, subZ - 0.9f}, Vec3{subX + 0.9f, 2.2f, subZ + 0.9f}, hvacMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{subX - 1.15f, 0.5f, subZ - 0.8f}, Vec3{subX - 0.9f, 2.0f, subZ + 0.8f}, darkMetalMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{subX + 0.9f, 0.5f, subZ - 0.8f}, Vec3{subX + 1.15f, 2.0f, subZ + 0.8f}, darkMetalMat);
    for (int ins = -1; ins <= 1; ++ins) {
        float ix = subX + ins * 0.5f;
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{ix - 0.12f, 2.2f, subZ - 0.12f}, Vec3{ix + 0.12f, 2.85f, subZ + 0.12f}, brickMat);
    }
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{subX - 0.25f, 1.2f, subZ + 0.91f}, Vec3{subX + 0.25f, 1.6f, subZ + 0.95f}, frameMat);
}

} // namespace procedural::buildings
