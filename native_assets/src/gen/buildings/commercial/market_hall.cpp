#include "TowerGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void TowerGen::generateMarketHall(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(26.0f, params_.footprintWidth);
    float d = std::max(18.0f, params_.footprintDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;
    float wallH = std::max(6.2f, params_.storyHeight * 1.4f);

    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t brickMat    = static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t limestone   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t frameMat    = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t glassMat    = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t signRed     = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t signOrange  = static_cast<uint32_t>(MaterialSlot::CommercialSignOrange);
    uint32_t turfMat     = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t pergolaMat  = static_cast<uint32_t>(MaterialSlot::PergolaWood);

    // 1. Pedestrian Cobblestone / Stone Market Plaza & Plinth
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 3.5f, -0.35f, -hd - 2.5f},
        Vec3{ hw + 3.5f,  0.00f,  hd + 4.5f},
        tarmacMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.6f, 0.00f, -hd - 0.6f},
        Vec3{ hw + 0.6f, 0.35f,  hd + 0.6f},
        foundMat);

    // 2. Victorian Brick & Wrought-Iron Arcaded Market Hall Envelope
    // Corner masonry piers
    float pierW = 2.4f;
    for (float sx : {-hw, hw - pierW}) {
        for (float sz : {-hd, hd - pierW}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{sx, 0.35f, sz},
                Vec3{sx + pierW, wallH, sz + pierW},
                brickMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{sx - 0.1f, wallH, sz - 0.1f},
                Vec3{sx + pierW + 0.1f, wallH + 0.45f, sz + pierW + 0.1f},
                limestone);
        }
    }

    // Glazed ironwork curtain arcades between corner piers on all 4 facades
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + pierW, 0.35f, hd - 0.25f},
        Vec3{ hw - pierW, wallH - 0.6f, hd + 0.05f},
        glassTinted);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + pierW, 0.35f, -hd - 0.05f},
        Vec3{ hw - pierW, wallH - 0.6f, -hd + 0.25f},
        glassTinted);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 0.05f, 0.35f, -hd + pierW},
        Vec3{-hw + 0.25f, wallH - 0.6f, hd - pierW},
        glassTinted);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{ hw - 0.25f, 0.35f, -hd + pierW},
        Vec3{ hw + 0.05f, wallH - 0.6f, hd - pierW},
        glassTinted);

    // Lower brick sill apron & cast-iron vertical arcade ribs
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.35f, hd - 0.3f},
        Vec3{ hw, 1.45f, hd + 0.12f},
        brickMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, wallH - 0.6f, -hd - 0.12f},
        Vec3{ hw, wallH + 0.2f,  hd + 0.12f},
        blackAlum);
    for (float mx = -hw + pierW; mx <= hw - pierW; mx += 2.6f) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mx - 0.10f, 0.35f, hd - 0.15f},
            Vec3{mx + 0.10f, wallH, hd + 0.16f},
            blackAlum);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mx - 0.10f, 0.35f, -hd - 0.16f},
            Vec3{mx + 0.10f, wallH, -hd + 0.15f},
            blackAlum);
    }

    // Grand Central Arched Market Portal Entrance (+Z)
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{0.0f, 0.35f, hd + 0.06f},
        4.2f, 3.6f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::CanopiedGrandPortal, true);

    // 3. Interior Artisanal Market Stalls (Visible through transparent arcade glass!)
    for (int s = -2; s <= 2; ++s) {
        float sx = s * (w * 0.16f);
        uint32_t awningMat = (s % 3 == 0) ? signRed : ((s % 2 == 0) ? turfMat : signOrange);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx - 1.1f, 0.35f, -2.5f},
            Vec3{sx + 1.1f, 1.35f,  2.5f},
            pergolaMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{sx - 1.25f, 2.35f, -2.6f},
            Vec3{sx + 1.25f, 2.65f,  2.6f},
            awningMat);
    }

    // 4. Vaulted Skylight Barrel Roof + Raised Clerestory Monitor Spine
    RoofGenParams roofParams;
    roofParams.type = RoofType::BarrelVault;
    roofParams.baseWidth = w;
    roofParams.baseDepth = d;
    roofParams.baseTopY = wallH;
    roofParams.roofHeight = 4.2f;
    roofParams.overhang = 0.5f;
    roofParams.showRooftopClutter = false;
    RoofGen::generate(outVerts, outIndices, roofParams);

    // Raised Central Glazed Clerestory Monitor Ridge running along X
    float ridgeY = wallH + roofParams.roofHeight - 0.35f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.72f, ridgeY, -1.6f},
        Vec3{ hw * 0.72f, ridgeY + 1.35f, 1.6f},
        glassMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.74f, ridgeY + 1.35f, -1.8f},
        Vec3{ hw * 0.74f, ridgeY + 1.65f,  1.8f},
        blackAlum);

    // 5. Exterior Farmers Market Canopy Stalls on Front Plaza (+Z)
    for (float ox : {-hw + 3.5f, hw - 3.5f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{ox - 1.8f, 0.00f, hd + 1.4f},
            Vec3{ox + 1.8f, 0.95f, hd + 2.8f},
            pergolaMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{ox - 1.95f, 2.25f, hd + 1.2f},
            Vec3{ox + 1.95f, 2.45f, hd + 3.0f},
            (ox < 0.0f) ? signRed : turfMat);
        for (float px : {ox - 1.75f, ox + 1.75f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{px - 0.06f, 0.0f, hd + 1.35f},
                Vec3{px + 0.06f, 2.25f, hd + 1.47f},
                frameMat);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{px - 0.06f, 0.0f, hd + 2.75f},
                Vec3{px + 0.06f, 2.25f, hd + 2.87f},
                frameMat);
        }
    }
}

} // namespace procedural::buildings
