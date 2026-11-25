#include "CivicGen.h"
#include "../BuildingComponents.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {
constexpr float kPi = 3.14159265358979323846f;
} // namespace

void CivicGen::generateKiccTower(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    // KICC — Kenyatta International Convention Centre, Nairobi (105m iconic cylindrical & Plenary Hall complex)
    float w = std::clamp(params_.mainWidth > 1.0f ? params_.mainWidth : 38.0f, 24.0f, 60.0f);
    float d = std::clamp(params_.mainDepth > 1.0f ? params_.mainDepth : 34.0f, 22.0f, 56.0f);
    int32_t stories = std::clamp(params_.stories, 16, 42);
    float storyH = std::clamp(params_.storyHeight > 0.5f ? params_.storyHeight : 3.4f, 2.8f, 4.8f);
    float cylinderTopY = std::max(stories * storyH, 95.2f);
    float saucerTopY = cylinderTopY + 5.2f;
    float totalH = std::max(saucerTopY + 4.6f, 105.0f);

    uint32_t terracottaStone = static_cast<uint32_t>(MaterialSlot::WallBrick);
    uint32_t creamLimestone  = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t darkWindow      = params_.useExtendedParams
        ? BuildingComponents::getGlassMaterial(params_.glassType)
        : static_cast<uint32_t>(MaterialSlot::GlassBronze);
    uint32_t darkSaucer      = static_cast<uint32_t>(MaterialSlot::WallBasalt);
    uint32_t crispWhite      = static_cast<uint32_t>(MaterialSlot::WallStucco);
    uint32_t tarmacMat       = static_cast<uint32_t>(MaterialSlot::TarmacDark);

    float towerR = std::clamp(w * 0.28f, 8.5f, 14.5f);
    float towerCenterX = -w * 0.16f;
    float podiumTopY = 8.4f;

    // 1. Ceremonial Concourse Plaza & Elevated KICC Podium Base (matching photo)
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-w * 0.72f, -0.22f, -d * 0.65f},
        Vec3{ w * 0.85f,  0.02f,  d * 0.65f},
        tarmacMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-w * 0.55f, 0.02f, -d * 0.48f},
        Vec3{ w * 0.65f, podiumTopY, d * 0.48f},
        creamLimestone);
    // Ornamental perimeter colonnade around plaza edge
    for (int f = -6; f <= 6; ++f) {
        float px = f * (w * 0.09f);
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{px - 0.35f, 0.02f, d * 0.46f},
            Vec3{px + 0.35f, podiumTopY, d * 0.52f},
            crispWhite);
    }

    // 2. ICONIC 28-STOREY CYLINDRICAL TERRACOTTA WAFFLE-GRID TOWER (offset left, 8.4m .. 95.2m)
    const int cylSegs = 24;
    // Recessed Inner Dark Bronze Glass Drum Cylinder
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{towerCenterX, podiumTopY, 0.0f},
        Vec3{towerCenterX, cylinderTopY, 0.0f},
        towerR * 0.90f, towerR * 0.90f, cylSegs, darkWindow, true, true
    );

    // 28 Distinct Overhanging Terracotta/Rose-Sandstone Floor Louver Rings + Vertical Mullion Fins!
    int floorRings = std::clamp(stories, 18, 32);
    float ringSpan = cylinderTopY - podiumTopY;
    for (int f = 0; f <= floorRings; ++f) {
        float ry = podiumTopY + f * (ringSpan / static_cast<float>(floorRings));
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            Vec3{towerCenterX, ry - 0.46f, 0.0f},
            Vec3{towerCenterX, ry + 0.46f, 0.0f},
            towerR * 1.02f, towerR * 1.02f, cylSegs,
            (f % 4 == 0) ? creamLimestone : terracottaStone, true, true
        );
    }

    // 20 Crisp Vertical Stone/Concrete Rib Fins forming KICC's Signature Circular Waffle Grid
    const int ribs = 20;
    for (int r = 0; r < ribs; ++r) {
        float ang = (2.0f * kPi * static_cast<float>(r)) / static_cast<float>(ribs);
        float rx = towerCenterX + std::cos(ang) * (towerR * 0.95f);
        float rz = std::sin(ang) * (towerR * 0.95f);
        BuildingComponents::appendOrientedBox(
            outVerts, outIndices,
            Vec3{rx, 0.5f * (podiumTopY + cylinderTopY), rz},
            Vec3{-std::sin(ang), 0.0f, std::cos(ang)},
            Vec3{0.0f, 1.0f, 0.0f},
            Vec3{std::cos(ang), 0.0f, std::sin(ang)},
            0.22f, ringSpan * 0.5f, 0.42f,
            terracottaStone
        );
    }

    // 3. WORLD-FAMOUS OVERHANGING CIRCULAR SAUCER HELIPAD / REVOLVING RESTAURANT CROWN (95m .. 105m)
    // Upper recessed observation gallery beneath saucer
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{towerCenterX, cylinderTopY, 0.0f},
        Vec3{towerCenterX, cylinderTopY + 2.2f, 0.0f},
        towerR * 0.86f, towerR * 0.86f, cylSegs, darkWindow, true, true
    );
    // Broad cantilevered flaring underside cone of the saucer disk
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{towerCenterX, cylinderTopY + 1.4f, 0.0f},
        Vec3{towerCenterX, saucerTopY - 1.2f, 0.0f},
        towerR * 0.94f, towerR * 1.38f, cylSegs, darkSaucer, true, true
    );
    // Flat Helipad Saucer Rim & Upper White Parapet Collar + Pinnacles up to 105.0m
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{towerCenterX, saucerTopY - 1.2f, 0.0f},
        Vec3{towerCenterX, saucerTopY + 0.4f, 0.0f},
        towerR * 1.38f, towerR * 1.32f, cylSegs, darkSaucer, true, true
    );
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{towerCenterX, saucerTopY + 0.4f, 0.0f},
        Vec3{towerCenterX, totalH, 0.0f},
        0.38f, 0.10f, 10, crispWhite, true, true
    );

    // 4. ICONIC CONICAL SLATTED PLENARY HALL AMPHITHEATER TENT beside Podium (+X side, matching photo right!)
    float tentX = w * 0.36f;
    float tentZ = d * 0.08f;
    float tentR = std::clamp(w * 0.25f, 7.5f, 12.5f);
    float tentApexY = podiumTopY + 18.5f;
    // Recessed dark octagonal base drum under cone overhang
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{tentX, podiumTopY, tentZ},
        Vec3{tentX, podiumTopY + 4.8f, tentZ},
        tentR * 0.78f, tentR * 0.78f, 16, darkSaucer, true, true
    );
    // Main Flared Conical Amphitheater Tent Shell
    BuildingComponents::appendOrientedCylinder(
        outVerts, outIndices,
        Vec3{tentX, podiumTopY + 4.4f, tentZ},
        Vec3{tentX, tentApexY, tentZ},
        tentR * 1.05f, 0.95f, 18, creamLimestone, true, true
    );
    // 12 Radiating Sloped Concrete Tent Ribs converging at Apex Lantern
    for (int rib = 0; rib < 12; ++rib) {
        float ang = (2.0f * kPi * static_cast<float>(rib)) / 12.0f;
        Vec3 basePt{tentX + std::cos(ang) * (tentR * 1.06f), podiumTopY + 4.3f, tentZ + std::sin(ang) * (tentR * 1.06f)};
        Vec3 apexPt{tentX + std::cos(ang) * 0.90f, tentApexY + 0.2f, tentZ + std::sin(ang) * 0.90f};
        BuildingComponents::appendOrientedCylinder(
            outVerts, outIndices,
            basePt, apexPt, 0.24f, 0.16f, 6, terracottaStone, true, true
        );
    }
}

} // namespace procedural::buildings
