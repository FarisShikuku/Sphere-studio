#include "CivicGen.h"
#include "../BuildingComponents.h"
#include "../parts/RoofGen.h"
#include <algorithm>
#include <cmath>

namespace procedural::buildings {

namespace {
void appendCross(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices,
    const Vec3& baseCenter,
    float height,
    float barW,
    uint32_t mat
) {
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{baseCenter.x - 0.08f, baseCenter.y, baseCenter.z - 0.08f},
        Vec3{baseCenter.x + 0.08f, baseCenter.y + height, baseCenter.z + 0.08f},
        mat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{baseCenter.x - barW * 0.5f, baseCenter.y + height * 0.65f, baseCenter.z - 0.07f},
        Vec3{baseCenter.x + barW * 0.5f, baseCenter.y + height * 0.65f + 0.14f, baseCenter.z + 0.07f},
        mat);
}
} // namespace

void CivicGen::generateGothicCathedral(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float w = std::max(22.0f, params_.mainWidth);
    float d = std::max(32.0f, params_.mainDepth);
    float hw = w * 0.5f;
    float hd = d * 0.5f;
    float naveH = 11.5f;
    float naveHW = std::min(6.2f, hw * 0.45f);

    uint32_t foundMat    = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t limestone   = static_cast<uint32_t>(MaterialSlot::WallLimestone);
    uint32_t corniceMat  = static_cast<uint32_t>(MaterialSlot::CorniceStone);
    uint32_t roofMat     = static_cast<uint32_t>(MaterialSlot::RoofSurface);
    uint32_t stainedMat  = static_cast<uint32_t>(MaterialSlot::GlassStained);
    uint32_t blackAlum   = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);
    uint32_t brassMat    = static_cast<uint32_t>(MaterialSlot::BrassHardware);

    // 1. Elevated Stone Cathedral Plaza & Steps
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw - 2.0f, -0.35f, -hd - 2.0f},
        Vec3{ hw + 2.0f,  0.00f,  hd + 4.5f},
        foundMat);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-hw + 1.0f, 0.0f, -hd},
        Vec3{ hw - 1.0f, 0.75f, hd + 1.8f},
        limestone);
    for (int s = 0; s < 4; ++s) {
        float sy = (s + 1) * (0.75f / 4.0f);
        float sz = hd + 3.2f - s * 0.35f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{-naveHW, 0.0f, sz - 0.36f},
            Vec3{ naveHW, sy,   sz},
            corniceMat);
    }

    // 2. Central Cruciform Nave Hall + Transept Wings
    float naveZ0 = -hd + 2.0f;
    float naveZ1 =  hd - 2.2f;
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-naveHW, 0.75f, naveZ0},
        Vec3{ naveHW, naveH, naveZ1},
        limestone);
    // Steep Gothic Gable Roof over Nave (rotated/extruded along Z)
    float ridgeH = 5.4f;
    // Left roof slope
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{-naveHW - 0.3f, naveH, naveZ1 + 0.2f},
        Vec3{-naveHW - 0.3f, naveH, naveZ0 - 0.2f},
        Vec3{0.0f, naveH + ridgeH, naveZ0 - 0.2f},
        Vec3{0.0f, naveH + ridgeH, naveZ1 + 0.2f},
        Vec3{-0.75f, 0.66f, 0.0f}, roofMat);
    // Right roof slope
    BuildingComponents::appendQuad(outVerts, outIndices,
        Vec3{ naveHW + 0.3f, naveH, naveZ0 - 0.2f},
        Vec3{ naveHW + 0.3f, naveH, naveZ1 + 0.2f},
        Vec3{0.0f, naveH + ridgeH, naveZ1 + 0.2f},
        Vec3{0.0f, naveH + ridgeH, naveZ0 - 0.2f},
        Vec3{ 0.75f, 0.66f, 0.0f}, roofMat);

    // Lower Side Aisles + Flying Buttress Piers along Left (-X) & Right (+X)
    float aisleOuterHW = naveHW + 3.2f;
    float aisleH = 6.2f;
    for (int side : {-1, 1}) {
        float ax0 = (side < 0) ? -aisleOuterHW : naveHW;
        float ax1 = (side < 0) ? -naveHW : aisleOuterHW;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{ax0, 0.75f, naveZ0 + 3.0f},
            Vec3{ax1, aisleH, naveZ1 - 3.5f},
            limestone);

        // Buttress piers & sloped flying buttress arches
        for (float bz = naveZ0 + 4.5f; bz <= naveZ1 - 4.5f; bz += 4.2f) {
            float pierX = (side < 0) ? (-aisleOuterHW - 0.5f) : (aisleOuterHW + 0.5f);
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{pierX - 0.45f, 0.75f, bz - 0.55f},
                Vec3{pierX + 0.45f, aisleH + 2.8f, bz + 0.55f},
                corniceMat);
            // Pinnacle spire on buttress
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{pierX, aisleH + 2.8f, bz},
                Vec3{pierX, aisleH + 4.6f, bz},
                0.35f, 0.04f, 4, corniceMat);
            // Diagonal Flying Buttress Strut bridging pier to high nave clerestory wall
            Vec3 pOuter{pierX, aisleH + 1.8f, bz};
            Vec3 pInner{(side < 0 ? -naveHW : naveHW), naveH - 1.2f, bz};
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                pOuter, pInner, 0.28f, 0.24f, 6, corniceMat);
        }

        // Clerestory Gothic Lancet Windows on Upper Nave Wall
        for (float wz = naveZ0 + 5.5f; wz <= naveZ1 - 5.5f; wz += 4.2f) {
            BuildingComponents::generateWindow(outVerts, outIndices,
                Vec3{(side < 0 ? -naveHW : naveHW), naveH - 2.6f, wz},
                1.35f, 2.8f,
                Vec3{static_cast<float>(side), 0.0f, 0.0f}, Vec3{0, 0, 1},
                WindowType::ArchedGothicLancet, GlassType::StainedGlassArtisan);
        }
    }

    // Transept Cross Arms (-X and +X near rear third)
    float trZ0 = -4.8f;
    float trZ1 =  2.2f;
    float trHW = std::min(hw - 0.5f, naveHW + 5.8f);
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-trHW, 0.75f, trZ0},
        Vec3{ trHW, naveH - 0.4f, trZ1},
        limestone);
    RoofGenParams trRoof;
    trRoof.type = RoofType::Gable;
    trRoof.baseWidth = trHW * 2.0f;
    trRoof.baseDepth = trZ1 - trZ0;
    trRoof.baseTopY = naveH - 0.4f;
    trRoof.roofHeight = 4.2f;
    trRoof.overhang = 0.2f;
    trRoof.showRooftopClutter = false;
    std::vector<BuildingVertex> trVerts;
    std::vector<uint32_t> trIdx;
    RoofGen::generate(trVerts, trIdx, trRoof);
    uint32_t vOff = static_cast<uint32_t>(outVerts.size());
    for (auto v : trVerts) {
        v.position.z += (trZ0 + trZ1) * 0.5f;
        v.materialId = roofMat;
        outVerts.push_back(v);
    }
    for (uint32_t idx : trIdx) outIndices.push_back(vOff + idx);

    // Transept Stained Glass Rose Windows
    for (int side : {-1, 1}) {
        BuildingComponents::generateWindow(outVerts, outIndices,
            Vec3{side * trHW, naveH - 3.6f, (trZ0 + trZ1) * 0.5f},
            3.2f, 3.2f, Vec3{static_cast<float>(side), 0.0f, 0.0f}, Vec3{0, 0, 1},
            WindowType::RoundOculus, GlassType::StainedGlassArtisan);
    }

    // 3. Monumental West Front Facade: Glowing Stained Glass Rose Window + Deep Portal
    BuildingComponents::generateDoor(outVerts, outIndices,
        Vec3{0.0f, 0.75f, naveZ1},
        3.6f, 4.4f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        DoorType::ArchedWooden, true);
    BuildingComponents::generateWindow(outVerts, outIndices,
        Vec3{0.0f, 8.2f, naveZ1 + 0.05f},
        4.2f, 4.2f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
        WindowType::RoundOculus, GlassType::StainedGlassArtisan);
    // Ornamental rose frame ring
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-2.2f, 5.8f, naveZ1 - 0.05f},
        Vec3{ 2.2f, 6.05f, naveZ1 + 0.35f},
        corniceMat);

    // 4. Twin Soaring Gothic Bell Towers & Octagonal Spires Flanking Front Entrance
    float towW = 4.2f;
    float towH = 19.5f;
    for (int side : {-1, 1}) {
        float tcx = side * (naveHW + 0.8f);
        float tcz = naveZ1 - 1.5f;
        BuildingComponents::appendBox(outVerts, outIndices,
            Vec3{tcx - towW * 0.5f, 0.75f, tcz - towW * 0.5f},
            Vec3{tcx + towW * 0.5f, towH,  tcz + towW * 0.5f},
            limestone);
        // Stone belt courses & Gothic belfry lancet openings on towers
        for (float by : {7.5f, 13.5f, towH}) {
            BuildingComponents::appendBox(outVerts, outIndices,
                Vec3{tcx - towW * 0.5f - 0.2f, by - 0.25f, tcz - towW * 0.5f - 0.2f},
                Vec3{tcx + towW * 0.5f + 0.2f, by + 0.20f, tcz + towW * 0.5f + 0.2f},
                corniceMat);
        }
        BuildingComponents::generateWindow(outVerts, outIndices,
            Vec3{tcx, 16.0f, tcz + towW * 0.5f},
            1.6f, 3.4f, Vec3{0, 0, 1}, Vec3{1, 0, 0},
            WindowType::ArchedGothicLancet, GlassType::StainedGlassArtisan);

        // Tall Octagonal Gothic Spire + Golden Cross Finial
        float spireH = 10.5f;
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{tcx, towH + 0.2f, tcz},
            Vec3{tcx, towH + 0.2f + spireH, tcz},
            towW * 0.46f, 0.08f, 8, roofMat);
        appendCross(outVerts, outIndices,
            Vec3{tcx, towH + 0.2f + spireH, tcz}, 2.1f, 1.1f, brassMat);
    }

    // Central Crossing Fleche Spire over Nave-Transept Intersection
    float crZ = (trZ0 + trZ1) * 0.5f;
    BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
        Vec3{0.0f, naveH + ridgeH - 0.5f, crZ},
        Vec3{0.0f, naveH + ridgeH + 9.5f, crZ},
        1.15f, 0.06f, 8, blackAlum);
    appendCross(outVerts, outIndices,
        Vec3{0.0f, naveH + ridgeH + 9.5f, crZ}, 1.6f, 0.85f, brassMat);
}

} // namespace procedural::buildings
