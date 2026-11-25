#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendCbkQuad(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3,
    uint32_t mat
) {
    Vec3 n = (p1 - p0).cross(p2 - p0);
    float len = n.length();
    n = (len > 1e-5f) ? (n / len) : Vec3{0.0f, 1.0f, 0.0f};
    BuildingComponents::appendQuad(outVerts, outIndices, p0, p1, p2, p3, n, mat);
}

} // namespace

void TowerGen::generateCbkPensionTower(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // CBK Pension Tower — Harambee Ave / CBD, Nairobi (110m crescent sail diagrid landmark)
    float w = std::clamp(params_.footprintWidth > 1.0f ? params_.footprintWidth : 34.0f, 22.0f, 52.0f);
    float d = std::clamp(params_.footprintDepth > 1.0f ? params_.footprintDepth : 26.0f, 18.0f, 44.0f);
    int32_t stories = std::clamp(params_.stories, 14, 45);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.7f, 3.0f, 5.0f);
    float totalH = std::max(stories * storyH + 10.1f, 110.0f);

    uint32_t silverAlum    = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t crispWhite    = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t cobaltGlass   = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t spandrelDark  = static_cast<uint32_t>(MaterialSlot::WallSpandrelGlass);
    uint32_t goldTruss     = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t tarmacMat     = static_cast<uint32_t>(MaterialSlot::TarmacDark);

    float hw = w * 0.5f;
    float hd = d * 0.5f;

    // 1. Slanted Trapezoidal Ground Podium, Entrance Portal Frame & Solar Deck
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 10.0f, -0.22f, -hd - 8.0f},
        Vec3{ hw + 10.0f,  0.02f,  hd + 10.0f},
        tarmacMat);

    float podiumH = 14.5f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.0f, 0.02f, -hd - 2.5f},
        Vec3{ hw + 4.0f, podiumH, hd + 3.5f},
        spandrelDark);
    // Silver horizontal podium cornice bands & rooftop solar PV array
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.3f, podiumH - 1.3f, -hd - 2.8f},
        Vec3{ hw + 4.3f, podiumH + 0.3f,  hd + 3.8f},
        silverAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw * 0.65f, podiumH + 0.3f, hd * 0.25f},
        Vec3{ hw * 0.65f, podiumH + 0.65f, hd + 2.2f},
        cobaltGlass);
    // Bold Diagonal Silver Front Portal Frame surrounding Ground Entrance
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{-hw * 0.45f, 0.2f, hd + 3.7f},
        Vec3{-hw * 0.34f, podiumH - 0.6f, hd + 3.7f},
        0.55f, 0.55f, 6, silverAlum, true, true
    );
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{ hw * 0.38f, 0.2f, hd + 3.7f},
        Vec3{ hw * 0.50f, podiumH - 0.6f, hd + 3.7f},
        0.55f, 0.55f, 6, silverAlum, true, true
    );
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{0.0f, 0.02f, hd + 3.65f},
        4.0f, 3.2f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::AutomaticSliding, true
    );

    // 2. Sweeping Curved Crescent Glass Core + Diagrid Exoskeleton (14.5m .. 96m)
    const int segs = 22;
    float curveSpanH = totalH - 12.0f - podiumH;
    auto crescentOffsetX = [&](float t) -> float {
        // Concave parabolic sweep curving outward toward top right (+X), matching uploaded photo
        return hw * 0.62f * (t * t - 0.48f * t);
    };

    float coreHalfW = hw * 0.44f;
    float coreHalfD = hd * 0.46f;

    float prevT = 0.0f;
    for (int s = 1; s <= segs; ++s) {
        float t = static_cast<float>(s) / static_cast<float>(segs);
        float y0 = podiumH + prevT * curveSpanH;
        float y1 = podiumH + t * curveSpanH;
        float xOff0 = crescentOffsetX(prevT);
        float xOff1 = crescentOffsetX(t);

        float taper0 = 1.0f - 0.10f * prevT;
        float taper1 = 1.0f - 0.10f * t;
        float w0 = coreHalfW * taper0;
        float w1 = coreHalfW * taper1;
        float d0 = coreHalfD * taper0;
        float d1 = coreHalfD * taper1;

        Vec3 b0{xOff0 - w0, y0,  d0}, b1{xOff0 + w0, y0,  d0}, b2{xOff0 + w0, y0, -d0}, b3{xOff0 - w0, y0, -d0};
        Vec3 t0{xOff1 - w1, y1,  d1}, t1{xOff1 + w1, y1,  d1}, t2{xOff1 + w1, y1, -d1}, t3{xOff1 - w1, y1, -d1};

        // Cobalt sapphire curved glass faces
        appendCbkQuad(outVerts, outIndices, b0, b1, t1, t0, cobaltGlass);
        appendCbkQuad(outVerts, outIndices, b1, b2, t2, t1, spandrelDark);
        appendCbkQuad(outVerts, outIndices, b2, b3, t3, t2, cobaltGlass);
        appendCbkQuad(outVerts, outIndices, b3, b0, t0, t3, spandrelDark);

        // Diamond Diagrid Mesh over Curved Front & Rear Glazing
        for (int db = 0; db < 4; ++db) {
            float f0 = static_cast<float>(db) / 4.0f;
            float f1 = static_cast<float>(db + 1) / 4.0f;
            for (float sz : {-1.0f, 1.0f}) {
                float dz0 = sz * (d0 + 0.12f);
                float dz1 = sz * (d1 + 0.12f);
                Vec3 pA{xOff0 - w0 + f0 * 2.0f * w0, y0, dz0};
                Vec3 pB{xOff0 - w0 + f1 * 2.0f * w0, y0, dz0};
                Vec3 pC{xOff1 - w1 + f0 * 2.0f * w1, y1, dz1};
                Vec3 pD{xOff1 - w1 + f1 * 2.0f * w1, y1, dz1};
                BuildingComponents::appendOrientedCylinder(outVerts, outIndices, pA, pD, 0.10f, 0.10f, 4, silverAlum, true, true);
                BuildingComponents::appendOrientedCylinder(outVerts, outIndices, pB, pC, 0.10f, 0.10f, 4, silverAlum, true, true);
            }
        }

        // 3. Twin Dramatic Sweeping Silver Crescent Sail Wings flanking Left (-X) & Right (+X) edges up to 110m!
        float wingTopH = totalH - podiumH;
        float wt0 = prevT * (wingTopH / curveSpanH);
        float wt1 = t * (wingTopH / curveSpanH);
        float wy0 = podiumH + prevT * wingTopH;
        float wy1 = podiumH + t * wingTopH;
        float wx0 = crescentOffsetX(std::min(wt0, 1.15f));
        float wx1 = crescentOffsetX(std::min(wt1, 1.15f));

        for (float side : {-1.0f, 1.0f}) {
            float bladeOff = side * (coreHalfW + 1.85f);
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{wx0 + bladeOff, wy0,  coreHalfD * 0.85f},
                Vec3{wx1 + bladeOff, wy1,  coreHalfD * 0.85f},
                0.75f, 0.72f, 8, crispWhite, true, true
            );
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{wx0 + bladeOff, wy0, -coreHalfD * 0.85f},
                Vec3{wx1 + bladeOff, wy1, -coreHalfD * 0.85f},
                0.75f, 0.72f, 8, silverAlum, true, true
            );
            // Connect front & rear wing blades as continuous solid architectural fin rib
            Vec3 lw0{wx0 + bladeOff - side * 0.55f, wy0,  coreHalfD * 0.85f};
            Vec3 lw1{wx0 + bladeOff - side * 0.55f, wy0, -coreHalfD * 0.85f};
            Vec3 lw2{wx1 + bladeOff - side * 0.55f, wy1, -coreHalfD * 0.85f};
            Vec3 lw3{wx1 + bladeOff - side * 0.55f, wy1,  coreHalfD * 0.85f};
            appendCbkQuad(outVerts, outIndices, lw0, lw1, lw2, lw3, silverAlum);
        }

        prevT = t;
    }

    // 4. Iconic Yellow Open Steel Space-Frame Crown Truss Bridging Crescent Wing Peaks at 110.0m
    float topX = crescentOffsetX(1.12f);
    float peakY = totalH;
    float roofY = podiumH + curveSpanH;
    Vec3 peakL{topX - coreHalfW - 1.85f, peakY, 0.0f};
    Vec3 peakR{topX + coreHalfW + 1.85f, peakY - 2.2f, 0.0f};
    Vec3 baseL{crescentOffsetX(1.0f) - coreHalfW, roofY, 0.0f};
    Vec3 baseR{crescentOffsetX(1.0f) + coreHalfW, roofY, 0.0f};

    for (float zOff : {-coreHalfD * 0.78f, 0.0f, coreHalfD * 0.78f}) {
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{peakL.x, peakL.y, zOff}, Vec3{peakR.x, peakR.y, zOff},
            0.35f, 0.35f, 6, silverAlum, true, true
        );
        for (int b = 0; b < 4; ++b) {
            float ft0 = static_cast<float>(b) / 4.0f;
            float ft1 = static_cast<float>(b + 1) / 4.0f;
            Vec3 topA{peakL.x + (peakR.x - peakL.x) * ft0, peakL.y + (peakR.y - peakL.y) * ft0, zOff};
            Vec3 topB{peakL.x + (peakR.x - peakL.x) * ft1, peakL.y + (peakR.y - peakL.y) * ft1, zOff};
            Vec3 botA{baseL.x + (baseR.x - baseL.x) * ft0, roofY, zOff};
            Vec3 botB{baseL.x + (baseR.x - baseL.x) * ft1, roofY, zOff};
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices, botA, topB, 0.18f, 0.18f, 5, goldTruss, true, true);
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices, botB, topA, 0.18f, 0.18f, 5, goldTruss, true, true);
        }
    }
}

} // namespace procedural::buildings
