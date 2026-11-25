#include "TowerGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>

namespace procedural::buildings {

void TowerGen::generatePetrolStation(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(22.0f, params_.footprintWidth);
    float d = std::max(16.0f, params_.footprintDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;
    float totalH = std::max(4.5f, params_.storyHeight);

    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t wallMat = static_cast<uint32_t>(MaterialSlot::WallConcrete);
    uint32_t glassMat = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t frameMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t roofMat = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t hvacMat = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t glassTintedMat = static_cast<uint32_t>(MaterialSlot::GlassTinted);

    // 1. Forecourt Asphalt / Concrete Foundation Apron with Generous Drive-Through Aprons (-X Entrance, +X Exit)
    float apronMinX = -hw - 4.5f;
    float apronMaxX = hw + 4.5f;
    float apronMinZ = -hd - 1.2f;
    float apronMaxZ = hd + 3.2f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{apronMinX, -0.4f, apronMinZ}, Vec3{apronMaxX, 0.0f, apronMaxZ}, foundMat);

    // 2. Convenience Store / Express Mart Building (Rear: compact depth to maximize forecourt drive space)
    float cStoreDepth = std::clamp(d * 0.28f, 3.2f, 5.5f);
    float cStoreMinZ = -hd;
    float cStoreMaxZ = -hd + cStoreDepth;
    float cStoreW = hw * 0.78f;
    float cStoreH = 3.6f;

    // Mart main envelope box
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-cStoreW, 0.0f, cStoreMinZ}, Vec3{cStoreW, cStoreH, cStoreMaxZ}, wallMat);

    // Storefront display windows & automatic sliding glass door facing +Z
    float winW = cStoreW * 0.55f;
    BuildingComponents::generateWindow(outVerts, outIndices, Vec3{-cStoreW * 0.55f, 1.8f, cStoreMaxZ}, winW, 2.2f, Vec3{0, 0, 1}, Vec3{1, 0, 0}, WindowType::PictureGlass, GlassType::ClearFloat);
    BuildingComponents::generateWindow(outVerts, outIndices, Vec3{ cStoreW * 0.55f, 1.8f, cStoreMaxZ}, winW, 2.2f, Vec3{0, 0, 1}, Vec3{1, 0, 0}, WindowType::PictureGlass, GlassType::ClearFloat);
    BuildingComponents::generateDoor(outVerts, outIndices, Vec3{0.0f, 0.0f, cStoreMaxZ}, 2.4f, 2.6f, Vec3{0, 0, 1}, Vec3{1, 0, 0}, DoorType::AutomaticSliding, true);

    // Storefront brand fascia sign board & logo canopy
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-cStoreW - 0.15f, cStoreH - 0.7f, cStoreMaxZ}, Vec3{cStoreW + 0.15f, cStoreH + 0.25f, cStoreMaxZ + 0.35f}, frameMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-cStoreW * 0.45f, cStoreH - 0.55f, cStoreMaxZ + 0.36f}, Vec3{cStoreW * 0.45f, cStoreH - 0.05f, cStoreMaxZ + 0.40f}, glassTintedMat);

    // Convenience store rooftop HVAC condensing units
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-2.5f, cStoreH, cStoreMinZ + 1.2f}, Vec3{-0.5f, cStoreH + 1.1f, cStoreMinZ + 3.0f}, hvacMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{ 1.0f, cStoreH, cStoreMinZ + 1.2f}, Vec3{ 2.8f, cStoreH + 0.8f, cStoreMinZ + 2.6f}, hvacMat);

    // Raised pedestrian sidewalk walkway in front of the store with safety bollards
    float sidewalkMaxZ = cStoreMaxZ + 1.2f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-cStoreW - 0.5f, 0.0f, cStoreMaxZ}, Vec3{cStoreW + 0.5f, 0.18f, sidewalkMaxZ}, foundMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-cStoreW - 0.52f, 0.14f, sidewalkMaxZ - 0.04f}, Vec3{cStoreW + 0.52f, 0.19f, sidewalkMaxZ + 0.02f}, frameMat);
    for (int b = -3; b <= 3; ++b) {
        float bx = b * (cStoreW * 0.28f);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{bx - 0.08f, 0.18f, sidewalkMaxZ - 0.15f}, Vec3{bx + 0.08f, 0.95f, sidewalkMaxZ - 0.01f}, frameMat);
    }

    // 3. Overhead Fuel Canopy & Forecourt Calculations
    float forecourtAvailableDepth = apronMaxZ - sidewalkMaxZ;
    float canopyDepth = std::clamp(forecourtAvailableDepth * 0.65f, 6.0f, 16.0f);
    float canopyMinZ = sidewalkMaxZ + (forecourtAvailableDepth - canopyDepth) * 0.45f;
    float canopyMaxZ = canopyMinZ + canopyDepth;
    float canopyW = hw * 0.95f;
    float canopyCenterZ = (canopyMinZ + canopyMaxZ) * 0.5f;
    float canopyDeckBottom = totalH;
    float canopyDeckTop = totalH + 0.75f;

    // Canopy roof fascia box
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-canopyW, canopyDeckBottom, canopyMinZ}, Vec3{canopyW, canopyDeckTop, canopyMaxZ}, frameMat);
    // Canopy top waterproofing deck
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-canopyW + 0.1f, canopyDeckTop - 0.02f, canopyMinZ + 0.1f}, Vec3{canopyW - 0.1f, canopyDeckTop + 0.05f, canopyMaxZ - 0.1f}, roofMat);
    // Underside illuminated ceiling soffit
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-canopyW + 0.2f, canopyDeckBottom - 0.02f, canopyMinZ + 0.2f}, Vec3{canopyW - 0.2f, canopyDeckBottom, canopyMaxZ - 0.2f}, glassMat);

    // Recessed LED lights under canopy
    for (int i = -2; i <= 2; ++i) {
        float lx = i * (canopyW * 0.38f);
        for (int j = 0; j <= 1; ++j) {
            float lz = canopyMinZ + (j + 0.5f) * ((canopyMaxZ - canopyMinZ) * 0.5f);
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{lx - 0.8f, canopyDeckBottom - 0.05f, lz - 0.5f}, Vec3{lx + 0.8f, canopyDeckBottom, lz + 0.5f}, glassTintedMat);
        }
    }

    // Corporate colored fascia accent runner band
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-canopyW - 0.05f, canopyDeckBottom + 0.25f, canopyMaxZ}, Vec3{canopyW + 0.05f, canopyDeckBottom + 0.45f, canopyMaxZ + 0.06f}, wallMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{-canopyW - 0.05f, canopyDeckBottom + 0.25f, canopyMinZ - 0.06f}, Vec3{canopyW + 0.05f, canopyDeckBottom + 0.45f, canopyMinZ}, wallMat);

    // 4. Adaptive Island Configuration (Single Grand Island when depth < 22m, Dual Islands when depth >= 22m)
    bool isDualIsland = (d >= 22.0f && forecourtAvailableDepth >= 14.0f);
    float islandHalfL = std::max(3.2f, canopyW * 0.68f);
    float islandHalfW = 0.55f; // 1.1m width in Z

    std::vector<float> islandZs;
    std::vector<float> laneZs;
    if (isDualIsland) {
        float islandSpacing = std::max(4.5f, (canopyMaxZ - canopyMinZ) * 0.35f);
        islandZs.push_back(canopyCenterZ + islandSpacing * 0.5f);
        islandZs.push_back(canopyCenterZ - islandSpacing * 0.5f);
        laneZs.push_back(islandZs[0] + 2.5f);
        laneZs.push_back(canopyCenterZ);
        laneZs.push_back(islandZs[1] - 2.5f);
    } else {
        islandZs.push_back(canopyCenterZ);
        laneZs.push_back(canopyCenterZ + 2.8f);
        laneZs.push_back(canopyCenterZ - 2.8f);
    }

    // Drive-Through Lanes Guidance Along X (East-West Drive-Through)
    for (float lz : laneZs) {
        int numDashes = 7;
        for (int d = 0; d < numDashes; ++d) {
            float dashX0 = apronMinX + 1.2f + d * ((apronMaxX - apronMinX - 3.0f) / numDashes);
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{dashX0, 0.0f, lz - 0.08f}, Vec3{dashX0 + 1.8f, 0.02f, lz + 0.08f}, frameMat);
        }
    }

    // 5. Fuel Islands & Multi-Product Dispensers
    for (float iz : islandZs) {
        // Concrete curb island along X
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-islandHalfL, 0.0f, iz - islandHalfW}, Vec3{islandHalfL, 0.25f, iz + islandHalfW}, foundMat);
        // Island safety yellow edge strip
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-islandHalfL - 0.03f, 0.20f, iz - islandHalfW - 0.03f}, Vec3{islandHalfL + 0.03f, 0.26f, iz + islandHalfW + 0.03f}, frameMat);

        // Crash pipe bollards at island ends (-X entrance, +X exit)
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{-islandHalfL - 0.22f, 0.25f, iz - 0.15f}, Vec3{-islandHalfL + 0.08f, 1.25f, iz + 0.15f}, frameMat);
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{ islandHalfL - 0.08f, 0.25f, iz - 0.15f}, Vec3{ islandHalfL + 0.22f, 1.25f, iz + 0.15f}, frameMat);

        // Integrated steel canopy columns on the islands
        for (float colX : {-canopyW * 0.42f, canopyW * 0.42f}) {
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{colX - 0.25f, 0.25f, iz - 0.25f}, Vec3{colX + 0.25f, canopyDeckBottom, iz + 0.25f}, frameMat);
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{colX - 0.35f, 0.25f, iz - 0.35f}, Vec3{colX + 0.35f, 1.15f, iz + 0.35f}, foundMat);
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{colX - 0.36f, 0.55f, iz - 0.36f}, Vec3{colX + 0.36f, 0.75f, iz + 0.36f}, frameMat);
        }

        // Multi-Product Fuel Dispensers spaced along X on each island
        std::vector<float> dispXs;
        if (islandHalfL >= 4.5f) {
            dispXs.push_back(-canopyW * 0.22f);
            dispXs.push_back( canopyW * 0.22f);
        } else {
            dispXs.push_back(0.0f);
        }

        for (float dispX : dispXs) {
            float dispX0 = dispX - 0.75f;
            float dispX1 = dispX + 0.75f;
            float dispHalfD = 0.22f;
            float dispH = 1.85f;

            // Dispenser body housing (along X)
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{dispX0, 0.25f, iz - dispHalfD}, Vec3{dispX1, dispH, iz + dispHalfD}, frameMat);
            // Corporate colored side panels
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{dispX0 - 0.03f, 0.25f, iz - dispHalfD + 0.03f}, Vec3{dispX0, dispH - 0.1f, iz + dispHalfD - 0.03f}, wallMat);
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{dispX1, 0.25f, iz - dispHalfD + 0.03f}, Vec3{dispX1 + 0.03f, dispH - 0.1f, iz + dispHalfD - 0.03f}, wallMat);

            // LCD digital price displays facing both drive lanes (-Z and +Z)
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{dispX - 0.40f, 1.15f, iz - dispHalfD - 0.04f}, Vec3{dispX + 0.40f, 1.60f, iz - dispHalfD}, glassTintedMat);
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{dispX - 0.40f, 1.15f, iz + dispHalfD}, Vec3{dispX + 0.40f, 1.60f, iz + dispHalfD + 0.04f}, glassTintedMat);

            // Hose retractor arms & nozzle holders on top
            BuildingComponents::appendBox(outVerts, outIndices, Vec3{dispX - 0.35f, dispH, iz - 0.18f}, Vec3{dispX + 0.35f, dispH + 0.42f, iz + 0.18f}, frameMat);
        }
    }

    // 6. Roadside Fuel Price Totem Pylon Sign at Entrance (-X entrance side)
    float totemX = apronMinX + 1.5f;
    float totemZ = canopyMaxZ + 0.6f;
    float totemW = 1.1f;
    float totemD = 0.45f;
    float totemH = 6.4f;

    // Totem base foundation plinth
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{totemX - totemW * 0.5f - 0.1f, 0.0f, totemZ - totemD * 0.5f - 0.1f}, Vec3{totemX + totemW * 0.5f + 0.1f, 0.35f, totemZ + totemD * 0.5f + 0.1f}, foundMat);
    // Pylon structural body
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{totemX - totemW * 0.5f, 0.35f, totemZ - totemD * 0.5f}, Vec3{totemX + totemW * 0.5f, totemH, totemZ + totemD * 0.5f}, frameMat);
    // Top illuminated brand logo header
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{totemX - totemW * 0.5f - 0.02f, totemH - 1.2f, totemZ - totemD * 0.5f - 0.02f}, Vec3{totemX + totemW * 0.5f + 0.02f, totemH, totemZ + totemD * 0.5f + 0.02f}, wallMat);

    // Multi-grade digital price panels (Regular, Plus, Diesel)
    for (int k = 0; k < 3; ++k) {
        float py = totemH - 1.8f - k * 0.95f;
        BuildingComponents::appendBox(outVerts, outIndices, Vec3{totemX - totemW * 0.42f, py, totemZ + totemD * 0.5f}, Vec3{totemX + totemW * 0.42f, py + 0.5f, totemZ + totemD * 0.5f + 0.05f}, glassTintedMat);
    }

    // 7. Tire Air & Water Utility Service Pedestal at Exit (+X exit side)
    float airX = apronMaxX - 1.8f;
    float airZ = canopyMaxZ + 0.6f;
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{airX - 0.35f, 0.0f, airZ - 0.35f}, Vec3{airX + 0.35f, 1.4f, airZ + 0.35f}, frameMat);
    BuildingComponents::appendBox(outVerts, outIndices, Vec3{airX - 0.25f, 0.9f, airZ + 0.35f}, Vec3{airX + 0.25f, 1.3f, airZ + 0.42f}, glassTintedMat);
}

} // namespace procedural::buildings
