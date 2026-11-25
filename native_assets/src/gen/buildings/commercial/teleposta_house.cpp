#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

void TowerGen::generateTelepostaHouse(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // Teleposta Towers — Kenyatta Avenue, Nairobi (120m iconic flared-crown tower)
    float w = std::clamp(params_.footprintWidth > 1.0f ? params_.footprintWidth : 30.0f, 20.0f, 48.0f);
    float d = std::clamp(params_.footprintDepth > 1.0f ? params_.footprintDepth : 30.0f, 20.0f, 48.0f);
    int32_t stories = std::clamp(params_.stories, 14, 48);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.85f, 3.0f, 5.2f);
    float shaftTopY = std::max(stories * storyH, 102.0f);
    float totalH = std::max(shaftTopY + 18.0f, 120.0f);

    uint32_t creamConcrete = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t warmBeige     = static_cast<uint32_t>(MaterialSlot::CardboardTan);
    uint32_t bronzeGlass   = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::GlassBronze);
    uint32_t darkFrame     = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t silverAlum    = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t tarmacMat     = static_cast<uint32_t>(MaterialSlot::TarmacDark);

    float hw = w * 0.5f;
    float hd = d * 0.5f;

    // 1. Kenyatta Ave Retail Banking Podium & Forecourt
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 8.0f, -0.22f, -hd - 7.0f},
        Vec3{ hw + 8.0f,  0.02f,  hd + 9.0f},
        tarmacMat);
    float podiumH = storyH * 2.4f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.0f, 0.02f, -hd - 2.5f},
        Vec3{ hw + 4.0f, podiumH, hd + 3.0f},
        creamConcrete);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.65f, 0.02f, hd + 2.95f},
        Vec3{ hw * 0.65f, podiumH - 1.2f, hd + 3.15f},
        bronzeGlass);
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{0.0f, 0.02f, hd + 3.2f},
        3.8f, 3.2f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::DoubleGlassCommercial, true
    );

    // 2. Signature Square Cream Concrete Shaft with Chamfered Corner Piers & Deep Recessed Bronze Window Rib Channels
    float coreHalfW = hw * 0.72f;
    float coreHalfD = hd * 0.72f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-coreHalfW, podiumH, -coreHalfD},
        Vec3{ coreHalfW, shaftTopY, coreHalfD},
        bronzeGlass);

    // Horizontal Dark Spandrel Rib Grid across All Recessed Window Channels
    for (int f = 3; f <= stories; ++f) {
        float fy = podiumH + (f - 2) * storyH;
        if (fy > shaftTopY - 1.0f) break;
        bool isMidReveal = (f == stories / 2);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-coreHalfW - 0.14f, fy - (isMidReveal ? 0.55f : 0.18f), -coreHalfD - 0.14f},
            Vec3{ coreHalfW + 0.14f, fy + (isMidReveal ? 0.55f : 0.18f),  coreHalfD + 0.14f},
            isMidReveal ? creamConcrete : darkFrame);
    }

    // Heavy Cream Precast Corner Bastion Piers & Intermediate Window Rib Mullions
    float cornerW = hw * 0.28f;
    for (float sx : {-1.0f, 1.0f}) {
        for (float sz : {-1.0f, 1.0f}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{sx * (hw - cornerW * 0.5f) - cornerW * 0.65f, podiumH, sz * (hd - cornerW * 0.5f) - cornerW * 0.65f},
                Vec3{sx * (hw - cornerW * 0.5f) + cornerW * 0.65f, shaftTopY, sz * (hd - cornerW * 0.5f) + cornerW * 0.65f},
                creamConcrete);
        }
    }
    // 4 Flanking Twin Vertical Window Pier Ribs on Every Facade (matching Teleposta photo)
    for (float px : {-0.45f, -0.15f, 0.15f, 0.45f}) {
        float xPos = px * hw;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{xPos - 0.42f, podiumH, -hd * 0.76f},
            Vec3{xPos + 0.42f, shaftTopY, hd * 0.76f},
            creamConcrete);
        float zPos = px * hd;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-hw * 0.76f, podiumH, zPos - 0.42f},
            Vec3{ hw * 0.76f, shaftTopY, zPos + 0.42f},
            creamConcrete);
    }

    // 3. Signature Teleposta Inverted-Pyramid Stepped Overhanging Flared Crown (102m .. 120m)
    float crownNeckY = shaftTopY + 3.4f;
    // Deep recessed dark penthouse band directly beneath overhang
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.78f, shaftTopY, -hd * 0.78f},
        Vec3{ hw * 0.78f, crownNeckY, hd * 0.78f},
        darkFrame);
    for (int col = -3; col <= 3; ++col) {
        float mx = col * (hw * 0.22f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mx - 0.35f, shaftTopY, -hd * 0.80f},
            Vec3{mx + 0.35f, crownNeckY, hd * 0.80f},
            warmBeige);
    }

    // Lower Wide Overhanging Crown Tier 1
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 1.12f, crownNeckY, -hd * 1.12f},
        Vec3{ hw * 1.12f, crownNeckY + 3.2f, hd * 1.12f},
        creamConcrete);
    // Upper Slightly Stepped Crown Tier 2
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 1.04f, crownNeckY + 3.2f, -hd * 1.04f},
        Vec3{ hw * 1.04f, crownNeckY + 6.0f,  hd * 1.04f},
        warmBeige);
    // Rooftop Mechanical Stepped Pavilion & Telecom Masts reaching 120.0m
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.55f, crownNeckY + 6.0f, -hd * 0.55f},
        Vec3{ hw * 0.55f, crownNeckY + 9.2f,  hd * 0.55f},
        creamConcrete);
    for (float sx : {-1.0f, 1.0f}) {
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{sx * hw * 0.28f, crownNeckY + 9.0f, 0.0f},
            Vec3{sx * hw * 0.28f, totalH,            0.0f},
            0.35f, 0.10f, 8, silverAlum, true, true
        );
    }
}

} // namespace procedural::buildings
