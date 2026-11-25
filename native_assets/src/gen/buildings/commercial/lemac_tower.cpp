#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void TowerGen::generateLemacTower(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // Le'Mac Tower — Westlands, Nairobi (126m chamfered corporate glass tower)
    float w = std::clamp(params_.footprintWidth > 1.0f ? params_.footprintWidth : 30.0f, 20.0f, 48.0f);
    float d = std::clamp(params_.footprintDepth > 1.0f ? params_.footprintDepth : 26.0f, 18.0f, 42.0f);
    int32_t stories = std::clamp(params_.stories, 14, 48);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 4.0f, 3.0f, 5.2f);
    float mainTopY = std::max(stories * storyH, 112.0f);
    float totalH = std::max(mainTopY + 14.0f, 126.0f);

    uint32_t cobaltGlass = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t silverAlum  = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t crispWhite  = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t frameDark   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);

    float hw = w * 0.5f;
    float hd = d * 0.5f;

    // 1. Landscaped Arcade Podium & Entrance Canopy
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 8.0f, -0.22f, -hd - 7.0f},
        Vec3{ hw + 8.0f,  0.02f,  hd + 9.0f},
        tarmacMat);
    float podiumH = storyH * 2.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.5f, 0.02f, -hd - 1.8f},
        Vec3{ hw + 2.5f, podiumH, hd + 1.8f},
        frameDark);
    // Double-height podium colonnade ribs & curved entrance canopy
    for (int c = -4; c <= 4; ++c) {
        float cx = c * (hw * 0.24f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{cx - 0.35f, 0.02f, hd + 1.5f},
            Vec3{cx + 0.35f, podiumH, hd + 2.3f},
            silverAlum);
    }
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.55f, podiumH - 0.4f, hd + 1.6f},
        Vec3{ hw * 0.55f, podiumH + 0.35f, hd + 5.2f},
        crispWhite);
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{0.0f, 0.02f, hd + 2.0f},
        3.6f, 3.2f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::AutomaticSliding, true
    );

    // 2. Main Chamfered-Corner Reflective Blue Glass Shaft
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.86f, podiumH, -hd * 0.86f},
        Vec3{ hw * 0.86f, mainTopY, hd * 0.86f},
        cobaltGlass);
    // Projecting Front-Center Glass Blade Bay & Rear Core Spine
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.42f, podiumH, hd * 0.60f},
        Vec3{ hw * 0.42f, mainTopY + 5.0f, hd * 0.96f},
        cobaltGlass);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.38f, podiumH, -hd * 0.96f},
        Vec3{ hw * 0.38f, mainTopY + 8.0f, -hd * 0.55f},
        frameDark);

    // 3. Rhythmic Projecting Horizontal Aluminum Floor Fins & Vertical Corner Spine Ribs
    for (int f = 3; f <= stories; ++f) {
        float fy = podiumH + (f - 2) * storyH;
        if (fy > mainTopY) break;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw * 0.90f, fy - 0.16f, -hd * 0.90f},
            Vec3{ hw * 0.90f, fy + 0.16f,  hd * 0.90f},
            crispWhite);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw * 0.44f, fy - 0.16f, hd * 0.62f},
            Vec3{ hw * 0.44f, fy + 0.16f, hd * 1.00f},
            silverAlum);
    }

    // Crisp Vertical Corner Chamfer Blades
    for (float sx : {-1.0f, 1.0f}) {
        for (float sz : {-1.0f, 1.0f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{sx * hw * 0.85f - 0.45f, podiumH, sz * hd * 0.85f - 0.45f},
                Vec3{sx * hw * 0.85f + 0.45f, mainTopY + 2.5f, sz * hd * 0.85f + 0.45f},
                silverAlum);
        }
    }

    // 4. Executive Penthouse Sky-Crown, Helipad Ring & Soaring Fin reaching 126.0m
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.56f, mainTopY, -hd * 0.56f},
        Vec3{ hw * 0.56f, totalH - 4.5f, hd * 0.56f},
        silverAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.52f, mainTopY + 1.2f, -hd * 0.52f},
        Vec3{ hw * 0.52f, totalH - 6.0f,  hd * 0.52f},
        cobaltGlass);
    // Helipad disk on crown deck + rooftop fin mast reaching 126m
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{0.0f, totalH - 4.5f, 0.0f},
        Vec3{0.0f, totalH - 3.6f, 0.0f},
        hw * 0.46f, hw * 0.46f, 18, crispWhite, true, true
    );
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{-hw * 0.25f, totalH - 3.6f, -hd * 0.25f},
        Vec3{-hw * 0.25f, totalH,        -hd * 0.25f},
        0.38f, 0.14f, 8, silverAlum, true, true
    );
}

} // namespace procedural::buildings
