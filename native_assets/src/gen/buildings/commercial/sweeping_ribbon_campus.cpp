#include "TowerGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace procedural::buildings {

namespace {

constexpr float kCampusPi = 3.14159265358979323846f;

std::vector<Vec2> makeRoundedRectLoopCCW(
    float cx, float cz,
    float width, float depth,
    float cornerRadius,
    int arcSegsPerCorner = 8
) {
    float hw = std::max(1.0f, width * 0.5f);
    float hd = std::max(1.0f, depth * 0.5f);
    float r = std::clamp(cornerRadius, 0.25f, std::min(hw, hd) - 0.1f);
    float ix = hw - r;
    float iz = hd - r;

    std::vector<Vec2> pts;
    pts.reserve(4 * (arcSegsPerCorner + 1));

    // Traverse CCW when viewed from +Y so (pNext - pCur) x (+Y) points strictly outward:
    // Quadrant 0: (+X, -Z) [angles 0 .. pi/2] using (ix + r*cos(a), -iz - r*sin(a))
    // Wait: at angle 0 -> (+hw, -iz); pi/2 -> (+ix, -hd). Both CCW!
    struct CornerSpec { float ox; float oz; float aStart; float aEnd; };
    CornerSpec corners[4] = {
        { ix, -iz, 0.0f,                  kCampusPi * 0.5f},       // Front-back? (+X, -Z)
        {-ix, -iz, kCampusPi * 0.5f,      kCampusPi},              // (-X, -Z)
        {-ix,  iz, kCampusPi,             kCampusPi * 1.5f},       // (-X, +Z)
        { ix,  iz, kCampusPi * 1.5f,      kCampusPi * 2.0f}        // (+X, +Z)
    };

    for (int c = 0; c < 4; ++c) {
        for (int i = 0; i <= arcSegsPerCorner; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(arcSegsPerCorner);
            float ang = corners[c].aStart + t * (corners[c].aEnd - corners[c].aStart);
            float px = cx + corners[c].ox + r * std::cos(ang);
            float pz = cz + corners[c].oz - r * std::sin(ang);
            if (!pts.empty()) {
                float dx = px - pts.back().x;
                float dz = pz - pts.back().y;
                if (dx * dx + dz * dz < 1e-6f) continue;
            }
            pts.push_back({px, pz});
        }
    }
    if (pts.size() > 2) {
        float dx = pts.front().x - pts.back().x;
        float dz = pts.front().y - pts.back().y;
        if (dx * dx + dz * dz < 1e-6f) pts.pop_back();
    }
    return pts;
}

void appendExtrudedRoundedBand(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float cx, float cz,
    float width, float depth,
    float y0, float y1,
    float cornerRadius,
    uint32_t materialId,
    bool addTopCap = true,
    bool addBottomCap = true,
    bool addVerticalMullions = false,
    uint32_t mullionMat = 0,
    float mullionSpacing = 2.4f
) {
    auto loop = makeRoundedRectLoopCCW(cx, cz, width, depth, cornerRadius, 8);
    int n = static_cast<int>(loop.size());
    if (n < 4) return;

    float accumLen = 0.0f;
    for (int i = 0; i < n; ++i) {
        const Vec2& p0 = loop[i];
        const Vec2& p1 = loop[(i + 1) % n];
        float dx = p1.x - p0.x;
        float dz = p1.y - p0.y;
        float segLen = std::sqrt(dx * dx + dz * dz);
        if (segLen < 1e-4f) continue;
        Vec3 normal{-dz / segLen, 0.0f, dx / segLen};

        BuildingComponents::appendQuad(
            outVerts, outIndices,
            Vec3{p0.x, y0, p0.y},
            Vec3{p1.x, y0, p1.y},
            Vec3{p1.x, y1, p1.y},
            Vec3{p0.x, y1, p0.y},
            normal, materialId
        );

        if (addVerticalMullions && segLen > 0.05f) {
            float prevBucket = std::floor(accumLen / mullionSpacing);
            float nextBucket = std::floor((accumLen + segLen) / mullionSpacing);
            if (nextBucket > prevBucket) {
                float mx = p0.x + normal.x * 0.04f;
                float mz = p0.y + normal.z * 0.04f;
                BuildingComponents::appendBox(
                    outVerts, outIndices,
                    Vec3{mx - 0.05f, y0, mz - 0.05f},
                    Vec3{mx + 0.05f, y1, mz + 0.05f},
                    mullionMat
                );
            }
        }
        accumLen += segLen;
    }

    if (addTopCap) {
        uint32_t cIdx = static_cast<uint32_t>(outVerts.size());
        outVerts.push_back({Vec3{cx, y1, cz}, Vec3{0, 1, 0}, {0.5f, 0.5f}, materialId});
        uint32_t rimStart = static_cast<uint32_t>(outVerts.size());
        for (int i = 0; i < n; ++i) {
            outVerts.push_back({Vec3{loop[i].x, y1, loop[i].y}, Vec3{0, 1, 0}, {0.0f, 0.0f}, materialId});
        }
        for (int i = 0; i < n; ++i) {
            outIndices.push_back(cIdx);
            outIndices.push_back(rimStart + i);
            outIndices.push_back(rimStart + ((i + 1) % n));
        }
    }

    if (addBottomCap) {
        uint32_t cIdx = static_cast<uint32_t>(outVerts.size());
        outVerts.push_back({Vec3{cx, y0, cz}, Vec3{0, -1, 0}, {0.5f, 0.5f}, materialId});
        uint32_t rimStart = static_cast<uint32_t>(outVerts.size());
        for (int i = 0; i < n; ++i) {
            outVerts.push_back({Vec3{loop[i].x, y0, loop[i].y}, Vec3{0, -1, 0}, {0.0f, 0.0f}, materialId});
        }
        for (int i = 0; i < n; ++i) {
            outIndices.push_back(cIdx);
            outIndices.push_back(rimStart + ((i + 1) % n));
            outIndices.push_back(rimStart + i);
        }
    }
}

/**
 * Generates a sculpted sweeping S-curve white facade ribbon band connecting an upper horizontal
 * facade spandrel smoothly down into the lower podium spandrel ledge.
 */
void appendSweepingSCurveRibbon(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float xStart, float yStart,
    float xEnd, float yEnd,
    float zBack, float zFront,
    float ribbonHeight,
    int segments,
    uint32_t materialId
) {
    float halfH = ribbonHeight * 0.5f;
    for (int i = 0; i < segments; ++i) {
        float t0 = static_cast<float>(i) / static_cast<float>(segments);
        float t1 = static_cast<float>(i + 1) / static_cast<float>(segments);

        // Cosine smoothstep S-curve interpolation
        float s0 = 0.5f * (1.0f - std::cos(kCampusPi * t0));
        float s1 = 0.5f * (1.0f - std::cos(kCampusPi * t1));

        float x0 = xStart + (xEnd - xStart) * t0;
        float y0 = yStart + (yEnd - yStart) * s0;
        float x1 = xStart + (xEnd - xStart) * t1;
        float y1 = yStart + (yEnd - yStart) * s1;

        // Tangent along ribbon center
        float dx = x1 - x0;
        float dy = y1 - y0;
        float len = std::max(1e-4f, std::sqrt(dx * dx + dy * dy));
        // Normal perpendicular to curve in XY plane
        float nx = -dy / len;
        float ny = dx / len;
        if (ny < 0.0f) { nx = -nx; ny = -ny; }

        Vec3 p0B{x0 - nx * halfH, y0 - ny * halfH, zFront};
        Vec3 p1B{x1 - nx * halfH, y1 - ny * halfH, zFront};
        Vec3 p1T{x1 + nx * halfH, y1 + ny * halfH, zFront};
        Vec3 p0T{x0 + nx * halfH, y0 + ny * halfH, zFront};

        // Front proud ribbon face (+Z)
        BuildingComponents::appendQuad(
            outVerts, outIndices,
            p0B, p1B, p1T, p0T,
            Vec3{0, 0, 1}, materialId
        );
        // Top beveled edge
        Vec3 b0T{p0T.x, p0T.y, zBack};
        Vec3 b1T{p1T.x, p1T.y, zBack};
        BuildingComponents::appendQuad(
            outVerts, outIndices,
            p0T, p1T, b1T, b0T,
            Vec3{nx, ny, 0.0f}, materialId
        );
        // Bottom soffit edge
        Vec3 b0B{p0B.x, p0B.y, zBack};
        Vec3 b1B{p1B.x, p1B.y, zBack};
        BuildingComponents::appendQuad(
            outVerts, outIndices,
            b0B, b1B, p1B, p0B,
            Vec3{-nx, -ny, 0.0f}, materialId
        );
    }
}

void appendCourtyardTree(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    float x, float y0, float z,
    float scale,
    uint32_t trunkMat, uint32_t leafMat
) {
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{x, y0, z},
        Vec3{x, y0 + 1.8f * scale, z},
        0.16f * scale, 0.11f * scale, 8, trunkMat, true, true
    );
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{x, y0 + 1.4f * scale, z},
        Vec3{x, y0 + 3.4f * scale, z},
        1.15f * scale, 0.55f * scale, 10, leafMat, true, true
    );
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{x + 0.4f * scale, y0 + 1.8f * scale, z - 0.25f * scale},
        Vec3{x + 0.4f * scale, y0 + 3.0f * scale, z - 0.25f * scale},
        0.85f * scale, 0.40f * scale, 8, leafMat, true, true
    );
}

} // namespace

void TowerGen::generateSweepingRibbonCampus(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::clamp(params_.footprintWidth > 0.5f ? params_.footprintWidth : 46.0f, 24.0f, 76.0f);
    float d = std::clamp(params_.footprintDepth > 0.5f ? params_.footprintDepth : 22.0f, 14.0f, 44.0f);
    int32_t totalStories = std::clamp(params_.stories, 5, 16);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.45f, 2.8f, 5.0f);

    uint32_t crispWhite  = (params_.useExtendedParams && params_.wallMaterial != WallMaterialType::GlassCurtainWall)
        ? BuildingComponents::getWallMaterial(params_.wallMaterial)
        : static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t blueGlass   = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::Glass);
    uint32_t warmGlass   = static_cast<uint32_t>(MaterialSlot::GlassGold);
    uint32_t frameMetal  = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t roofMat     = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t hvacMat     = static_cast<uint32_t>(MaterialSlot::HvacMetal);
    uint32_t tarmacMat   = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t greenTurf   = static_cast<uint32_t>(MaterialSlot::GreenTurfDeck);
    uint32_t planterBox  = static_cast<uint32_t>(MaterialSlot::PlanterBox);
    uint32_t timberMat   = static_cast<uint32_t>(MaterialSlot::WallTimber);

    float hw = w * 0.5f;
    float hd = d * 0.5f;

    // 1. Forecourt Plaza, Parking Apron & Perimeter Curbs
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 6.0f, -0.20f, -hd - 4.0f},
        Vec3{ hw + 6.0f,  0.02f,  hd + 10.0f},
        tarmacMat);
    // Paved white-stone sidewalk apron directly in front of podium
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 4.5f, 0.02f, -hd - 2.2f},
        Vec3{ hw + 4.5f, 0.16f,  hd + 4.8f},
        static_cast<uint32_t>(MaterialSlot::WallLimestone));

    // Landscaped front lawn planter strips along plaza edge
    for (float px : {-hw * 0.52f, hw * 0.45f}) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 4.8f, 0.16f, hd + 3.2f},
            Vec3{px + 4.8f, 0.42f, hd + 4.5f},
            planterBox);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 4.6f, 0.42f, hd + 3.35f},
            Vec3{px + 4.6f, 0.54f, hd + 4.35f},
            greenTurf);
    }

    // 2. Wide 2-Story Horizontal Rounded-Corner Podium Plinth Base
    float podW = w * 1.04f;
    float podD = d * 1.08f;
    float podH = 2.0f * storyH;
    float podY0 = 0.16f;
    float podBandH = storyH;

    for (int pf = 0; pf < 2; ++pf) {
        float y0 = podY0 + pf * podBandH;
        float glassTop = y0 + podBandH * 0.56f;
        float spandrelTop = y0 + podBandH;
        float tierShrink = pf * 1.4f;

        // Horizontal louvered glass strip
        appendExtrudedRoundedBand(
            outVerts, outIndices,
            0.0f, 0.5f, podW - 0.5f - tierShrink, podD - 0.5f - tierShrink,
            y0, glassTop, 2.6f,
            warmGlass, false, false, true, blackAlum, 2.2f
        );
        // Horizontal dark louvre strip inside podium ribbon
        appendExtrudedRoundedBand(
            outVerts, outIndices,
            0.0f, 0.5f, podW - 0.42f - tierShrink, podD - 0.42f - tierShrink,
            y0 + podBandH * 0.25f, y0 + podBandH * 0.34f, 2.6f,
            blackAlum, false, false
        );
        // Crisp rounded white podium fascia band
        appendExtrudedRoundedBand(
            outVerts, outIndices,
            0.0f, 0.5f, podW - tierShrink, podD - tierShrink,
            glassTop, spandrelTop, 2.8f,
            crispWhite, true, true
        );
    }

    // Central Recessed Main Entrance Atrium + White Arrival Canopy
    float entW = std::min(9.0f, w * 0.24f);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-entW * 0.5f, podY0, podD * 0.5f - 0.4f},
        Vec3{ entW * 0.5f, podY0 + 3.2f, podD * 0.5f + 0.7f},
        blackAlum);
    BuildingComponents::generateDoor(
        outVerts, outIndices,
        Vec3{0.0f, podY0, podD * 0.5f + 0.72f},
        std::min(4.6f, entW * 0.65f), 2.85f,
        Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::AutomaticSliding, true
    );

    // Podium Terrace Perimeter Green Planter Strip (under upper wing setback)
    float upperBaseY = podY0 + podH;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-podW * 0.42f, upperBaseY, -podD * 0.35f},
        Vec3{ podW * 0.42f, upperBaseY + 0.32f, podD * 0.42f},
        greenTurf);

    // 3. Main Upper Rounded-Corner Wing (Left/Center Dominant Mass)
    int mainUpperFloors = std::max(3, totalStories - 2);
    float mainW = w * 0.64f;
    float mainD = d * 0.74f;
    float mainCX = -w * 0.14f;
    float mainCZ = 0.0f;
    float cornerR = std::min(3.4f, mainD * 0.22f);

    float spandrelRatio = 0.42f;
    float glassH = storyH * (1.0f - spandrelRatio);
    float spandrelH = storyH * spandrelRatio;

    for (int f = 0; f < mainUpperFloors; ++f) {
        float fy = upperBaseY + f * storyH;
        // White rounded floor spandrel ribbon at base of each tier
        appendExtrudedRoundedBand(
            outVerts, outIndices,
            mainCX, mainCZ, mainW, mainD,
            fy, fy + spandrelH * 0.72f, cornerR,
            crispWhite, true, true
        );
        // Recessed continuous blue curtain-wall glazing band with dark vertical mullions
        appendExtrudedRoundedBand(
            outVerts, outIndices,
            mainCX, mainCZ, mainW - 0.55f, mainD - 0.55f,
            fy + spandrelH * 0.72f, fy + spandrelH * 0.72f + glassH, std::max(1.2f, cornerR - 0.3f),
            blueGlass, false, false, true, frameMetal, 2.3f
        );
    }

    // Tall Rounded White Parapet Crown above Main Upper Wing
    float mainTopY = upperBaseY + mainUpperFloors * storyH;
    float crownH = std::max(1.35f, storyH * 0.52f);
    appendExtrudedRoundedBand(
        outVerts, outIndices,
        mainCX, mainCZ, mainW + 0.06f, mainD + 0.06f,
        mainTopY - 0.35f, mainTopY + crownH, cornerR,
        crispWhite, true, true
    );
    // Dark recessed roof deck cap inside main wing crown
    appendExtrudedRoundedBand(
        outVerts, outIndices,
        mainCX, mainCZ, mainW - 1.2f, mainD - 1.2f,
        mainTopY + crownH - 0.15f, mainTopY + crownH + 0.05f, std::max(1.0f, cornerR - 0.6f),
        roofMat, true, false
    );

    // 4. Secondary Stepped Upper Wing (Right / Offset Rear)
    int secFloors = std::max(2, mainUpperFloors - 1);
    float secW = w * 0.52f;
    float secD = d * 0.68f;
    float secCX = w * 0.24f;
    float secCZ = -d * 0.18f;
    float secR = std::min(3.0f, secD * 0.22f);

    for (int f = 0; f < secFloors; ++f) {
        float fy = upperBaseY + f * storyH;
        appendExtrudedRoundedBand(
            outVerts, outIndices,
            secCX, secCZ, secW, secD,
            fy, fy + spandrelH * 0.72f, secR,
            crispWhite, true, true
        );
        appendExtrudedRoundedBand(
            outVerts, outIndices,
            secCX, secCZ, secW - 0.55f, secD - 0.55f,
            fy + spandrelH * 0.72f, fy + spandrelH * 0.72f + glassH, std::max(1.1f, secR - 0.3f),
            blueGlass, false, false, true, frameMetal, 2.3f
        );
    }
    float secTopY = upperBaseY + secFloors * storyH;
    appendExtrudedRoundedBand(
        outVerts, outIndices,
        secCX, secCZ, secW + 0.06f, secD + 0.06f,
        secTopY - 0.35f, secTopY + crownH, secR,
        crispWhite, true, true
    );

    // 5. SIGNATURE SWEEPING PARAMETRIC S-CURVE WHITE FACADE RIBBONS
    // Sweeps diagonally across the front facade of Main Upper Wing connecting top crown down to podium ledge
    float frontFaceZ = mainCZ + mainD * 0.5f;
    float rX0 = mainCX - mainW * 0.34f;
    float rX1 = mainCX - mainW * 0.06f;

    // Upper sweeping S-curve white ribbon descending from crown down into middle ribbon
    appendSweepingSCurveRibbon(
        outVerts, outIndices,
        rX0, mainTopY - 0.15f,
        rX1 + 2.5f, upperBaseY + storyH * 1.25f,
        frontFaceZ - 0.35f, frontFaceZ + 0.42f,
        0.72f, 18, crispWhite
    );
    // Lower sweeping S-curve white ribbon peeling from mid-left down smoothly onto podium rim
    appendSweepingSCurveRibbon(
        outVerts, outIndices,
        rX0 - 1.8f, upperBaseY + storyH * 2.35f,
        rX1 - 0.6f, upperBaseY + 0.32f,
        frontFaceZ - 0.35f, frontFaceZ + 0.44f,
        0.78f, 18, crispWhite
    );

    // 6. Plaza Landscaping Trees (along front apron plaza, clear of entrance doors)
    float treePositions[4] = {-hw * 0.72f, -hw * 0.30f, hw * 0.30f, hw * 0.72f};
    for (int i = 0; i < 4; ++i) {
        appendCourtyardTree(
            outVerts, outIndices,
            treePositions[i], 0.16f, podD * 0.5f + 2.6f,
            1.15f + (i % 2) * 0.18f, timberMat, greenTurf
        );
    }

    // 7. Rooftop Mechanical HVAC Penthouse (when clutter enabled)
    if (!params_.useExtendedParams || params_.showRooftopClutter) {
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{mainCX - 3.6f, mainTopY + crownH, mainCZ - 2.2f},
            Vec3{mainCX + 3.6f, mainTopY + crownH + 1.65f, mainCZ + 2.2f},
            hvacMat);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{secCX - 2.4f, secTopY + crownH, secCZ - 1.6f},
            Vec3{secCX + 2.4f, secTopY + crownH + 1.35f, secCZ + 1.6f},
            hvacMat);
    }
}

} // namespace procedural::buildings
