#include "CivicGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void CivicGen::generateTrainStation(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(30.0f, params_.mainWidth);
    float d = std::max(24.0f, params_.mainDepth);
    float h = std::max(10.0f, params_.stories * params_.storyHeight);
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    uint32_t stoneMat   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t foundMat   = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t frameMat   = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t roofMat    = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t glassMat   = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t ballastMat = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t steelMat   = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t yellowMat  = static_cast<uint32_t>(MaterialSlot::SafetyYellow);

    // 1. Grand Station Foundation Plinth
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-hw - 0.8f, -0.6f, -hd - 0.8f},
        Vec3{ hw + 0.8f,  0.0f,  hd + 0.8f},
        foundMat
    );

    // 2. Grand Neoclassical Station Headhouse (Front portion +Z)
    float headW = w * 0.9f;
    float headD = d * 0.38f;
    float headH = h;
    float headMinX = -headW * 0.5f;
    float headMaxX =  headW * 0.5f;
    float headMinZ =  hd - headD;
    float headMaxZ =  hd;

    // Headhouse stone structure
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{headMinX, 0.0f, headMinZ},
        Vec3{headMaxX, headH, headMaxZ},
        stoneMat
    );

    // Monumental Roman Arch Portico Entrance (+Z)
    float porticoW = headW * 0.45f;
    float porticoH = headH * 0.85f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-porticoW * 0.5f, 0.0f, headMaxZ},
        Vec3{ porticoW * 0.5f, porticoH, headMaxZ + 1.2f},
        foundMat
    );
    // Portico grand entrance doors
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{0.0f, 0.0f, headMaxZ + 1.2f},
        3.8f, 3.4f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::ArchedWooden, true
    );

    // Grand Entrance Station Clock (Embedded in upper pediment)
    float clockY = headH * 0.72f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-1.3f, clockY - 1.3f, headMaxZ + 1.22f},
        Vec3{ 1.3f, clockY + 1.3f, headMaxZ + 1.35f},
        frameMat
    );
    // Clock face glass
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-1.0f, clockY - 1.0f, headMaxZ + 1.36f},
        Vec3{ 1.0f, clockY + 1.0f, headMaxZ + 1.4f},
        glassMat
    );

    // Flanking Arched Windows on Headhouse
    for (int story = 0; story < params_.stories; ++story) {
        float winY = story * params_.storyHeight + 1.6f;
        BuildingComponents::generateWindow(
            outVerts, outIndices,
            Vec3{-headW * 0.35f, winY, headMaxZ},
            2.0f, 2.2f,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            WindowType::ArchedHistoric
        );
        BuildingComponents::generateWindow(
            outVerts, outIndices,
            Vec3{ headW * 0.35f, winY, headMaxZ},
            2.0f, 2.2f,
            Vec3{0, 0, 1}, Vec3{1, 0, 0},
            WindowType::ArchedHistoric
        );
    }

    // Headhouse Mansard Roof
    RoofGenParams hRoof;
    hRoof.type = RoofType::Mansard;
    hRoof.baseWidth = headW;
    hRoof.baseDepth = headD;
    hRoof.baseTopY = headH;
    hRoof.roofHeight = 3.2f;
    RoofGen::generate(outVerts, outIndices, hRoof);

    // 3. Grand Arched Glass & Steel Train Shed (Rear portion -Z)
    float shedW = headW * 0.95f;
    float shedD = d - headD + 2.0f;
    float shedMinZ = -hd;
    float shedMaxZ = headMinZ;
    float shedApexH = headH * 1.15f;
    float shedSpringH = headH * 0.45f;

    // Side structural arched clerestory walls
    // West wall (-X)
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-shedW * 0.5f - 0.5f, 0.0f, shedMinZ},
        Vec3{-shedW * 0.5f, shedSpringH, shedMaxZ},
        stoneMat
    );
    // East wall (+X)
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{shedW * 0.5f, 0.0f, shedMinZ},
        Vec3{shedW * 0.5f + 0.5f, shedSpringH, shedMaxZ},
        stoneMat
    );

    // Curved Arched Barrel-Vault Canopy Ribs & Glass Skylights
    int numRibs = 7;
    float ribSpacing = shedD / (numRibs - 1);
    for (int r = 0; r < numRibs; ++r) {
        float rz = shedMinZ + r * ribSpacing;

        // Structural curved steel arch rib (approximated with 5 segments)
        float segs = 5;
        float prevX = -shedW * 0.5f;
        float prevY = shedSpringH;

        for (int s = 1; s <= segs; ++s) {
            float t = static_cast<float>(s) / segs;
            float curX = -shedW * 0.5f + t * shedW;
            // Parabolic arch profile
            float normX = (t - 0.5f) * 2.0f; // -1 to +1
            float curY = shedSpringH + (shedApexH - shedSpringH) * (1.0f - normX * normX);

            BuildingComponents::appendBox(
                outVerts, outIndices,
                Vec3{prevX - 0.2f, std::min(prevY, curY) - 0.2f, rz - 0.25f},
                Vec3{curX + 0.2f,  std::max(prevY, curY) + 0.2f, rz + 0.25f},
                frameMat
            );

            prevX = curX;
            prevY = curY;
        }
    }

    // Glass Skylight Panels spanning between the ribs
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-shedW * 0.45f, shedSpringH + 1.2f, shedMinZ},
        Vec3{ shedW * 0.45f, shedApexH, shedMaxZ},
        glassMat
    );

    // 4. Dual Raised Passenger Boarding Platforms & Sunken Railway Tracks
    float platW = shedW * 0.26f;
    float trackBedW = shedW * 0.28f;

    // Platform 1 (Left -X)
    float plat1CenterX = -shedW * 0.28f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{plat1CenterX - platW * 0.5f, 0.0f, shedMinZ},
        Vec3{plat1CenterX + platW * 0.5f, 0.45f, shedMaxZ},
        foundMat
    );
    // Platform 1 Safety Yellow Edge
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{plat1CenterX + platW * 0.5f - 0.25f, 0.45f, shedMinZ},
        Vec3{plat1CenterX + platW * 0.5f, 0.46f, shedMaxZ},
        yellowMat
    );

    // Platform 2 (Right +X)
    float plat2CenterX = shedW * 0.28f;
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{plat2CenterX - platW * 0.5f, 0.0f, shedMinZ},
        Vec3{plat2CenterX + platW * 0.5f, 0.45f, shedMaxZ},
        foundMat
    );
    // Platform 2 Safety Yellow Edge
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{plat2CenterX - platW * 0.5f, 0.45f, shedMinZ},
        Vec3{plat2CenterX - platW * 0.5f + 0.25f, 0.46f, shedMaxZ},
        yellowMat
    );

    // Sunken Railway Track Bed (Center) with ballast gravel
    BuildingComponents::appendBox(
        outVerts, outIndices,
        Vec3{-trackBedW * 0.5f, -0.25f, shedMinZ - 4.0f},
        Vec3{ trackBedW * 0.5f,  0.0f,  shedMaxZ},
        ballastMat
    );

    // Parallel Steel Rails (Dual Tracks)
    float railPositions[4] = {-2.2f, -1.0f, 1.0f, 2.2f};
    for (int r = 0; r < 4; ++r) {
        float rx = railPositions[r];
        BuildingComponents::appendBox(
            outVerts, outIndices,
            Vec3{rx - 0.06f, 0.0f, shedMinZ - 4.0f},
            Vec3{rx + 0.06f, 0.12f, shedMaxZ},
            steelMat
        );
    }
}

} // namespace procedural::buildings
