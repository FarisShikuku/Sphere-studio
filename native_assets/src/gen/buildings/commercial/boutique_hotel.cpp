#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>

namespace procedural::buildings {

void TowerGen::generateBoutiqueHotel(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(24.0f, params_.footprintWidth);
    float d = std::max(20.0f, params_.footprintDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;
    int32_t stories = std::clamp(params_.stories, 2, 6);
    float flH = std::max(3.0f, params_.storyHeight);

    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t gunmetal    = static_cast<uint32_t>(MaterialSlot::WallGunmetalAluminum);
    uint32_t champAlum   = static_cast<uint32_t>(MaterialSlot::WallChampagneAluminum);
    uint32_t cortenMat   = static_cast<uint32_t>(MaterialSlot::WallCorten);
    uint32_t stuccoMat   = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t pergolaMat  = static_cast<uint32_t>(MaterialSlot::PergolaWood);
    uint32_t poolMat     = static_cast<uint32_t>(MaterialSlot::GlassEmerald);
    uint32_t glassMat    = static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t glassTinted = static_cast<uint32_t>(MaterialSlot::GlassTinted);
    uint32_t balconyGlass= static_cast<uint32_t>(MaterialSlot::BalconyGlass);
    uint32_t roofMat     = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t turfMat     = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t planterMat  = static_cast<uint32_t>(MaterialSlot::PlanterBox);

    // 1. Resort Courtyard Concrete & Timber Terrace Plinth
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 1.0f, -0.35f, -hd - 1.0f},
        Vec3{ hw + 1.0f,  0.00f,  hd + 1.0f},
        foundMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw, 0.0f, -hd},
        Vec3{ hw, 0.25f, hd},
        pergolaMat);

    // Central Courtyard Resort Pool (Front-Center Courtyard Void)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.40f, 0.25f, -1.5f},
        Vec3{ hw * 0.40f, 0.36f,  hd - 2.2f},
        foundMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.36f, 0.36f, -1.1f},
        Vec3{ hw * 0.36f, 0.42f,  hd - 2.6f},
        poolMat);

    // 2. Ground Floor Glazed Check-In Lobby & Lounge Wing (Rear -Z across full width)
    float lobbyZ0 = -hd + 0.4f;
    float lobbyZ1 = -2.8f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.5f, 0.25f, lobbyZ0},
        Vec3{ hw - 0.5f, flH,   lobbyZ1},
        blackAlum);
    // Floor-to-ceiling courtyard lobby glass & canopy entrance
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 2.0f, 0.40f, lobbyZ1 - 0.1f},
        Vec3{ hw - 2.0f, flH - 0.3f, lobbyZ1 + 0.06f},
        glassTinted);
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{0.0f, 0.25f, lobbyZ1},
        2.6f, 2.5f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::AutomaticSliding, true);

    // 3. Staggered Modular Container Hotel Suite Matrix (U-Shaped Wings & Stacked Upper Floors)
    uint32_t moduleMats[4] = {gunmetal, champAlum, cortenMat, stuccoMat};
    int rearSuites = 4;
    float suiteW = (w - 1.6f) / static_cast<float>(rearSuites);
    float suiteD = 6.2f;

    for (int f = 1; f < stories; ++f) {
        float y0 = f * flH;
        float y1 = (f + 1) * flH - 0.12f;

        // Open-air steel walkway gallery behind suites (-Z side)
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 0.8f, y0 - 0.12f, lobbyZ0 - 0.8f},
            Vec3{ hw - 0.8f, y0,          lobbyZ0 + 0.2f},
            blackAlum);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw + 0.8f, y0, lobbyZ0 - 0.8f},
            Vec3{ hw - 0.8f, y0 + 1.0f, lobbyZ0 - 0.72f},
            balconyGlass);

        for (int s = 0; s < rearSuites; ++s) {
            float sx0 = -hw + 0.8f + s * suiteW + 0.18f;
            float sx1 = sx0 + suiteW - 0.36f;
            // Alternate cantilever stagger forward over courtyard on odd/even floors!
            float staggerZ = ((f + s) % 2 == 0) ? 1.2f : 0.0f;
            float sz0 = lobbyZ0 + staggerZ;
            float sz1 = sz0 + suiteD;
            uint32_t mat = moduleMats[(f + s) % 4];

            // Modular Suite Box
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{sx0, y0, sz0},
                Vec3{sx1, y1, sz1},
                mat);
            // Panoramic suite window wall facing courtyard (+Z)
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{sx0 + 0.35f, y0 + 0.35f, sz1 - 0.05f},
                Vec3{sx1 - 0.35f, y1 - 0.30f, sz1 + 0.06f},
                glassTinted);
            // Private cantilevered glass balcony deck on every suite
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{sx0 + 0.2f, y0 - 0.10f, sz1},
                Vec3{sx1 - 0.2f, y0 + 0.05f, sz1 + 1.35f},
                blackAlum);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{sx0 + 0.2f, y0 + 0.05f, sz1 + 1.28f},
                Vec3{sx1 - 0.2f, y0 + 1.05f, sz1 + 1.35f},
                balconyGlass);
        }

        // Flanking Left (-X) & Right (+X) Wing Suites enclosing courtyard pool
        for (int side : {-1, 1}) {
            float wx0 = (side < 0) ? (-hw + 0.6f) : (hw - 6.2f);
            float wx1 = (side < 0) ? (-hw + 6.2f) : (hw - 0.6f);
            float wz0 = lobbyZ1 + 0.6f;
            float wz1 = hd - 1.8f;
            uint32_t wingMat = moduleMats[(f + (side > 0 ? 1 : 2)) % 4];
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{wx0, y0, wz0},
                Vec3{wx1, y1, wz1},
                wingMat);
            // Courtyard-facing suite windows
            float winFaceX = (side < 0) ? wx1 : wx0;
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{winFaceX, (y0 + y1) * 0.5f, (wz0 + wz1) * 0.5f},
                (wz1 - wz0) * 0.65f, flH * 0.65f,
                Vec3{side < 0 ? 1.0f : -1.0f, 0.0f, 0.0f}, Vec3{0, 0, 1},
                WindowType::PictureGlass, GlassType::ClearFloat);
        }
    }

    // 4. Rooftop Sky-Lounge Bar Pergola & Green Terrace Garden
    float topY = stories * flH;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 0.7f, topY - 0.1f, lobbyZ0},
        Vec3{ hw - 0.7f, topY + 0.15f, lobbyZ0 + suiteD + 1.0f},
        roofMat);
    // Rooftop glass pergola pavilion
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-5.0f, topY + 0.15f, lobbyZ0 + 0.8f},
        Vec3{ 5.0f, topY + 2.85f, lobbyZ0 + 4.5f},
        glassMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-5.4f, topY + 2.85f, lobbyZ0 + 0.5f},
        Vec3{ 5.4f, topY + 3.15f, lobbyZ0 + 4.8f},
        pergolaMat);
    // Rooftop green planter boxes
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.2f, topY + 0.15f, lobbyZ0 + 0.8f},
        Vec3{-6.0f,      topY + 0.65f, lobbyZ0 + 3.5f},
        planterMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.3f, topY + 0.65f, lobbyZ0 + 0.9f},
        Vec3{-6.1f,      topY + 1.00f, lobbyZ0 + 3.4f},
        turfMat);
}

} // namespace procedural::buildings
