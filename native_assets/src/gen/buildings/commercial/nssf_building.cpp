#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void TowerGen::generateNssfBuilding(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // NSSF Building (Social Security House) — Bishops Road, Nairobi (103m twin-slab modernist landmark)
    float w = std::clamp(params_.footprintWidth > 1.0f ? params_.footprintWidth : 40.0f, 26.0f, 60.0f);
    float d = std::clamp(params_.footprintDepth > 1.0f ? params_.footprintDepth : 26.0f, 18.0f, 44.0f);
    int32_t stories = std::clamp(params_.stories, 12, 42);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.45f, 2.8f, 4.8f);
    float towerTopY = std::max(stories * storyH, 96.0f);
    float totalH = std::max(towerTopY + 7.0f, 103.0f);

    uint32_t precastWhite  = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t creamStone    = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t bronzeGlass   = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::GlassBronze);
    uint32_t darkFrame     = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t tarmacMat     = static_cast<uint32_t>(MaterialSlot::TarmacDark);

    float hw = w * 0.5f;
    float hd = d * 0.5f;

    // 1. Broad 3-Storey Bishops Road Brutalist-Modernist Podium
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 9.0f, -0.22f, -hd - 7.0f},
        Vec3{ hw + 9.0f,  0.02f,  hd + 9.0f},
        tarmacMat);
    float podiumH = storyH * 3.0f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.0f, 0.02f, -hd - 2.5f},
        Vec3{ hw + 4.0f, podiumH, hd + 2.5f},
        creamStone);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.7f, 0.02f, hd + 2.4f},
        Vec3{ hw * 0.7f, podiumH - 1.5f, hd + 2.6f},
        bronzeGlass);
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{0.0f, 0.02f, hd + 2.65f},
        4.4f, 3.2f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true
    );

    // 2. Central Vertical Concrete Elevator Core Spine Linking Block A & Block B
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.20f, podiumH, -hd * 0.52f},
        Vec3{ hw * 0.20f, totalH,   hd * 0.52f},
        creamStone);

    // 3. Staggered Twin Office Slabs (Block A on Left/Front, Block B on Right/Rear — classic Social Security House massing)
    struct SlabBox { float x0, x1, z0, z1, topY; };
    SlabBox slabs[2] = {
        {-hw * 0.96f, -hw * 0.14f, -hd * 0.42f,  hd * 0.84f, towerTopY},
        { hw * 0.14f,  hw * 0.96f, -hd * 0.84f,  hd * 0.42f, towerTopY - 3.5f}
    };

    for (const auto& s : slabs) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{s.x0, podiumH, s.z0},
            Vec3{s.x1, s.topY,  s.z1},
            bronzeGlass);

        // Dense Vertical Precast White Concrete Mullion Fins
        int cols = 7;
        for (int c = 0; c <= cols; ++c) {
            float fx = s.x0 + c * ((s.x1 - s.x0) / static_cast<float>(cols));
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{fx - 0.22f, podiumH, s.z0 - 0.18f},
                Vec3{fx + 0.22f, s.topY + 0.8f, s.z1 + 0.18f},
                precastWhite);
        }

        // Bold Horizontal Spandrel Rib Bands every floor
        for (int f = 3; f <= stories; ++f) {
            float fy = podiumH + (f - 2) * storyH;
            if (fy > s.topY) break;
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{s.x0 - 0.14f, fy - 0.28f, s.z0 - 0.14f},
                Vec3{s.x1 + 0.14f, fy + 0.28f, s.z1 + 0.14f},
                (f % 2 == 0) ? precastWhite : darkFrame);
        }

        // Solid End Shear Walls & Mechanical Rooftop Penthouse
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{s.x0 - 0.3f, podiumH, s.z0},
            Vec3{s.x0 + 0.8f, s.topY + 2.5f, s.z1},
            creamStone);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{s.x1 - 0.8f, podiumH, s.z0},
            Vec3{s.x1 + 0.3f, s.topY + 2.5f, s.z1},
            creamStone);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{0.5f * (s.x0 + s.x1) - 3.5f, s.topY, 0.5f * (s.z0 + s.z1) - 3.2f},
            Vec3{0.5f * (s.x0 + s.x1) + 3.5f, totalH, 0.5f * (s.z0 + s.z1) + 3.2f},
            precastWhite);
    }
}

} // namespace procedural::buildings
