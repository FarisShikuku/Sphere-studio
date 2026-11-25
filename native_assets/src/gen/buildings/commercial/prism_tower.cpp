#include "TowerGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {

void appendPrismQuad(
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

void appendTriPrismFace(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    const Vec3& a, const Vec3& b, const Vec3& c,
    uint32_t mat
) {
    Vec3 n = (b - a).cross(c - a);
    float len = n.length();
    n = (len > 1e-5f) ? (n / len) : Vec3{0.0f, 1.0f, 0.0f};
    uint32_t idx = static_cast<uint32_t>(outVerts.size());
    outVerts.push_back({a, n, {0.0f, 0.0f}, mat});
    outVerts.push_back({b, n, {1.0f, 0.0f}, mat});
    outVerts.push_back({c, n, {0.5f, 1.0f}, mat});
    outIndices.push_back(idx);
    outIndices.push_back(idx + 1);
    outIndices.push_back(idx + 2);
}

} // namespace

void TowerGen::generatePrismTower(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // Prism Tower — Upper Hill, Nairobi (133m twisting crystalline prism landmark)
    float w = std::clamp(params_.footprintWidth > 1.0f ? params_.footprintWidth : 30.0f, 20.0f, 48.0f);
    float d = std::clamp(params_.footprintDepth > 1.0f ? params_.footprintDepth : 26.0f, 18.0f, 44.0f);
    int32_t stories = std::clamp(params_.stories, 16, 50);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.7f, 3.0f, 5.0f);
    float totalH = std::max(stories * storyH + 7.2f, 133.0f);

    uint32_t crispWhite    = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t amberPodium   = static_cast<uint32_t>(MaterialSlot::WallCorten);
    uint32_t silverAlum    = static_cast<uint32_t>(MaterialSlot::WallSilverAluminum);
    uint32_t crystalGlass  = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::GlassReflective);
    uint32_t spandrelDark  = static_cast<uint32_t>(MaterialSlot::WallSpandrelGlass);
    uint32_t redRoofSign   = static_cast<uint32_t>(MaterialSlot::CommercialAccentRed);
    uint32_t tarmacMat     = static_cast<uint32_t>(MaterialSlot::TarmacDark);

    float hw = w * 0.5f;
    float hd = d * 0.5f;

    // 1. Signature Amber/Terracotta X-Braced Diagrid Parking Podium (0m .. 19m)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 12.0f, -0.22f, -hd - 8.0f},
        Vec3{ hw + 12.0f,  0.02f,  hd + 10.0f},
        tarmacMat);

    float podiumH = 19.0f;
    float podHw = hw + 8.5f;
    float podHd = hd + 2.5f;
    // Perforated light silver screen behind bold amber X-diagrid trusses
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-podHw, 0.02f, -podHd},
        Vec3{ podHw, podiumH, podHd},
        silverAlum);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-podHw - 0.1f, podiumH - 1.1f, -podHd - 0.1f},
        Vec3{ podHw + 0.1f, podiumH + 0.4f,  podHd + 0.1f},
        amberPodium);

    // Bold Amber Diagonal X-Trusses across Front & Rear Podium
    int xBays = 6;
    float bayW = (2.0f * podHw) / static_cast<float>(xBays);
    for (float sz : {-1.0f, 1.0f}) {
        float pz = sz * (podHd + 0.25f);
        for (int b = 0; b < xBays; ++b) {
            float x0 = -podHw + b * bayW;
            float x1 = x0 + bayW;
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{x0, 0.4f, pz}, Vec3{x1, podiumH - 0.8f, pz},
                0.52f, 0.52f, 6, amberPodium, true, true
            );
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices,
                Vec3{x1, 0.4f, pz}, Vec3{x0, podiumH - 0.8f, pz},
                0.52f, 0.52f, 6, amberPodium, true, true
            );
        }
    }

    // 2. Adjacent Rear Limestone Core Spine (visible behind left of Prism Tower in photo)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.0f, podiumH, -hd * 0.9f},
        Vec3{-hw * 0.5f, totalH * 0.58f, -hd * 0.1f},
        static_cast<uint32_t>(MaterialSlot::WallLimestone));

    // 3. Seven Zigzag Stacked Twisting Crystalline Glass Prism Segments (19m .. 133m)
    const int tiers = 7;
    float towerSpanH = totalH - 4.5f - podiumH;
    float tierH = towerSpanH / static_cast<float>(tiers);

    auto getCornersAtTier = [&](int tierIdx) -> std::array<Vec3, 4> {
        float t = static_cast<float>(tierIdx) / static_cast<float>(tiers);
        float y = podiumH + t * towerSpanH;
        // Zigzag oscillating angular offset + slight upward taper
        float zig = (tierIdx % 2 == 0) ? -1.0f : 1.0f;
        float angleRad = zig * 0.28f + t * 0.22f;
        float rx = (hw * 0.86f) * (1.0f - 0.14f * t);
        float rz = (hd * 0.82f) * (1.0f - 0.14f * t);
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        auto rotPt = [&](float lx, float lz) -> Vec3 {
            // Add asymmetric cantilever kick in alternating direction for deconstructivist facets
            float kickX = zig * 2.2f;
            return Vec3{lx * c - lz * s + kickX, y, lx * s + lz * c};
        };
        return {
            rotPt(-rx,  rz), // 0: front-left
            rotPt( rx,  rz), // 1: front-right
            rotPt( rx, -rz), // 2: rear-right
            rotPt(-rx, -rz)  // 3: rear-left
        };
    };

    auto bottomCorners = getCornersAtTier(0);
    for (int seg = 1; seg <= tiers; ++seg) {
        auto topCorners = getCornersAtTier(seg);

        // Triangulated crystalline facets per side (splits each quad cleanly along diagonal)
        for (int face = 0; face < 4; ++face) {
            int nextF = (face + 1) % 4;
            const Vec3& b0 = bottomCorners[face];
            const Vec3& b1 = bottomCorners[nextF];
            const Vec3& t0 = topCorners[face];
            const Vec3& t1 = topCorners[nextF];

            // Alternate triangulation diagonal between odd/even tiers to produce iconic Prism zigzag
            if ((seg + face) % 2 == 0) {
                appendTriPrismFace(outVerts, outIndices, b0, b1, t1, crystalGlass);
                appendTriPrismFace(outVerts, outIndices, b0, t1, t0, spandrelDark);
                // Bold crisp white diagonal crease mullion
                BuildingComponents::appendOrientedCylinder(
                    outVerts, outIndices, b0, t1, 0.24f, 0.24f, 6, crispWhite, true, true
                );
            } else {
                appendTriPrismFace(outVerts, outIndices, b0, b1, t0, crystalGlass);
                appendTriPrismFace(outVerts, outIndices, b1, t1, t0, spandrelDark);
                // Bold crisp white diagonal crease mullion
                BuildingComponents::appendOrientedCylinder(
                    outVerts, outIndices, b1, t0, 0.24f, 0.24f, 6, crispWhite, true, true
                );
            }

            // White horizontal tier seam ring & crisp vertical ridge spine
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices, t0, t1, 0.22f, 0.22f, 6, crispWhite, true, true
            );
            BuildingComponents::appendOrientedCylinder(
                outVerts, outIndices, b0, t0, 0.28f, 0.28f, 6, crispWhite, true, true
            );
        }

        // Subdivide internal diamond glass mullions along each segment height
        for (int mid = 1; mid <= 3; ++mid) {
            float ft = static_cast<float>(mid) / 4.0f;
            for (int face = 0; face < 4; ++face) {
                int nextF = (face + 1) % 4;
                Vec3 pA{
                    bottomCorners[face].x + (topCorners[face].x - bottomCorners[face].x) * ft,
                    bottomCorners[face].y + ft * tierH,
                    bottomCorners[face].z + (topCorners[face].z - bottomCorners[face].z) * ft
                };
                Vec3 pB{
                    bottomCorners[nextF].x + (topCorners[nextF].x - bottomCorners[nextF].x) * ft,
                    bottomCorners[nextF].y + ft * tierH,
                    bottomCorners[nextF].z + (topCorners[nextF].z - bottomCorners[nextF].z) * ft
                };
                BuildingComponents::appendOrientedCylinder(
                    outVerts, outIndices, pA, pB, 0.08f, 0.08f, 4, silverAlum, true, true
                );
            }
        }

        bottomCorners = topCorners;
    }

    // 4. Tilted Red-Accented Rooftop Parapet Crown reaching 133.0m
    auto crownBase = getCornersAtTier(tiers);
    std::array<Vec3, 4> crownTop = crownBase;
    crownTop[0].y = totalH;
    crownTop[1].y = totalH - 1.2f;
    crownTop[2].y = totalH - 2.4f;
    crownTop[3].y = totalH - 0.8f;

    for (int f = 0; f < 4; ++f) {
        int nf = (f + 1) % 4;
        appendPrismQuad(outVerts, outIndices, crownBase[f], crownBase[nf], crownTop[nf], crownTop[f],
            (f == 0) ? redRoofSign : crispWhite);
    }
    appendPrismQuad(outVerts, outIndices, crownTop[0], crownTop[1], crownTop[2], crownTop[3], crispWhite);
}

} // namespace procedural::buildings
