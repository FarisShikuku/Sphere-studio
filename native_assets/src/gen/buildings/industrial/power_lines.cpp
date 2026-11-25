#include "WarehouseGen.h"
#include "../BuildingComponents.h"
#include <cmath>
#include <algorithm>
#include <vector>

namespace procedural::buildings {

void WarehouseGen::generatePowerLines(
    std::vector<BuildingVertex>& outVerts,
    std::vector<uint32_t>& outIndices
) {
    float length = std::max(36.0f, std::max(params_.length, params_.width));
    float width = std::max(16.0f, std::min(params_.length, params_.width));
    float poleHeight = std::max(10.0f, params_.height);

    float halfLen = length * 0.5f;
    float halfW = width * 0.5f;

    uint32_t tarmacMat = static_cast<uint32_t>(MaterialSlot::TarmacDark);
    uint32_t foundMat = static_cast<uint32_t>(MaterialSlot::Foundation);
    uint32_t metalMat = static_cast<uint32_t>(MaterialSlot::FrameMetal);
    uint32_t timberMat = static_cast<uint32_t>(MaterialSlot::WallTimber);
    uint32_t ceramicMat = static_cast<uint32_t>(MaterialSlot::GlassBronze);
    uint32_t yellowMat = static_cast<uint32_t>(MaterialSlot::SafetyYellow);
    uint32_t cableMat = static_cast<uint32_t>(MaterialSlot::WallBlackAluminum);

    // 1. Utility Corridor Ground Right-of-Way Easement
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-halfW, -0.3f, -halfLen},
        Vec3{ halfW,  0.0f,  halfLen},
        foundMat);

    // Maintenance access gravel/tarmac service track along the easement center
    BuildingComponents::appendBox(outVerts, outIndices,
        Vec3{-2.4f, 0.0f, -halfLen},
        Vec3{ 2.4f, 0.04f, halfLen},
        tarmacMat);

    // Safety bollards at corridor entry/exit
    float bollardRadius = 0.12f;
    for (float bz : {-halfLen + 1.2f, halfLen - 1.2f}) {
        for (float bx : {-2.6f, 2.6f}) {
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{bx, 0.0f, bz}, Vec3{bx, 0.95f, bz},
                bollardRadius, bollardRadius, 10, yellowMat, true, true);
        }
    }

    // 2. Three High-Voltage Transmission Utility Posts spaced along Z corridor
    std::vector<float> poleZ = {
        -halfLen * 0.68f,
        0.0f,
        halfLen * 0.68f
    };

    struct InsulatorAttachment {
        Vec3 pos;
    };
    std::vector<std::vector<InsulatorAttachment>> postAttachments(poleZ.size());

    float poleBaseRadius = 0.22f;
    float poleTopRadius = 0.14f;
    float armWidthUpper = 3.6f;
    float armWidthLower = 4.4f;

    for (size_t pIdx = 0; pIdx < poleZ.size(); ++pIdx) {
        float pz = poleZ[pIdx];

        // Footing collar (continuous cylinder)
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{0.0f, 0.0f, pz}, Vec3{0.0f, 0.5f, pz},
            poleBaseRadius * 1.5f, poleBaseRadius * 1.25f,
            16, foundMat, true, true);

        // Main tapered vertical pole (continuous cylinder)
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{0.0f, 0.5f, pz}, Vec3{0.0f, poleHeight, pz},
            poleBaseRadius, poleTopRadius,
            18, timberMat, false, true);

        // Lower crossarm (continuous horizontal cylinder)
        float lowerArmY = poleHeight - 2.1f;
        float hArmL = armWidthLower * 0.5f;
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{-hArmL, lowerArmY, pz}, Vec3{hArmL, lowerArmY, pz},
            0.075f, 0.075f,
            14, timberMat, true, true);

        // Diagonal brace struts for lower crossarm (continuous cylinders)
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{-hArmL * 0.65f, lowerArmY - 0.05f, pz + 0.05f},
            Vec3{0.0f, lowerArmY - 0.75f, pz + 0.08f},
            0.024f, 0.024f,
            10, metalMat, true, true);
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{hArmL * 0.65f, lowerArmY - 0.05f, pz + 0.05f},
            Vec3{0.0f, lowerArmY - 0.75f, pz + 0.08f},
            0.024f, 0.024f,
            10, metalMat, true, true);

        // Upper crossarm (continuous horizontal cylinder)
        float upperArmY = poleHeight - 0.9f;
        float hArmU = armWidthUpper * 0.5f;
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{-hArmU, upperArmY, pz}, Vec3{hArmU, upperArmY, pz},
            0.075f, 0.075f,
            14, timberMat, true, true);

        // Diagonal brace struts for upper crossarm (continuous cylinders)
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{-hArmU * 0.65f, upperArmY - 0.05f, pz + 0.05f},
            Vec3{0.0f, upperArmY - 0.65f, pz + 0.08f},
            0.024f, 0.024f,
            10, metalMat, true, true);
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{hArmU * 0.65f, upperArmY - 0.05f, pz + 0.05f},
            Vec3{0.0f, upperArmY - 0.65f, pz + 0.08f},
            0.024f, 0.024f,
            10, metalMat, true, true);

        // Insulators on lower crossarm: outer ends (2 positions)
        for (float ix : {-hArmL * 0.88f, hArmL * 0.88f}) {
            float baseY = lowerArmY + 0.075f;
            float topY = baseY + 0.28f;
            // Pin cylinder
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{ix, baseY, pz}, Vec3{ix, topY, pz},
                0.03f, 0.024f, 10, metalMat, false, true);
            // Ceramic shed skirts (stacked flared cylinders)
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{ix, baseY + 0.06f, pz}, Vec3{ix, baseY + 0.14f, pz},
                0.085f, 0.060f, 12, ceramicMat, true, true);
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{ix, baseY + 0.16f, pz}, Vec3{ix, baseY + 0.24f, pz},
                0.075f, 0.050f, 12, ceramicMat, true, true);

            postAttachments[pIdx].push_back(InsulatorAttachment{Vec3{ix, topY + 0.04f, pz}});
        }

        // Insulators on upper crossarm: outer ends (2 positions)
        for (float ix : {-hArmU * 0.86f, hArmU * 0.86f}) {
            float baseY = upperArmY + 0.075f;
            float topY = baseY + 0.28f;
            // Pin cylinder
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{ix, baseY, pz}, Vec3{ix, topY, pz},
                0.03f, 0.024f, 10, metalMat, false, true);
            // Ceramic shed skirts (stacked flared cylinders)
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{ix, baseY + 0.06f, pz}, Vec3{ix, baseY + 0.14f, pz},
                0.085f, 0.060f, 12, ceramicMat, true, true);
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{ix, baseY + 0.16f, pz}, Vec3{ix, baseY + 0.24f, pz},
                0.075f, 0.050f, 12, ceramicMat, true, true);

            postAttachments[pIdx].push_back(InsulatorAttachment{Vec3{ix, topY + 0.04f, pz}});
        }

        // Top apex lightning shield wire mount (continuous cylinder)
        float apexY = poleHeight + 0.35f;
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{0.0f, poleHeight, pz}, Vec3{0.0f, apexY, pz},
            0.026f, 0.020f, 10, metalMat, false, true);
        BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
            Vec3{0.0f, poleHeight + 0.10f, pz}, Vec3{0.0f, poleHeight + 0.22f, pz},
            0.065f, 0.050f, 12, ceramicMat, true, true);
        postAttachments[pIdx].push_back(InsulatorAttachment{Vec3{0.0f, apexY + 0.02f, pz}});

        // Pole-mounted step-down transformer on center post
        if (pIdx == 1) {
            float transY = poleHeight - 3.2f;
            // Transformer cylindrical tank (continuous cylinder)
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{0.0f, transY - 0.65f, pz + 0.52f},
                Vec3{0.0f, transY + 0.65f, pz + 0.52f},
                0.32f, 0.32f, 18, metalMat, true, true);

            // High voltage porcelain bushings (continuous angled cylinders)
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{-0.15f, transY + 0.65f, pz + 0.52f},
                Vec3{-0.22f, transY + 0.98f, pz + 0.52f},
                0.045f, 0.028f, 10, ceramicMat, true, true);
            BuildingComponents::appendOrientedCylinder(outVerts, outIndices,
                Vec3{0.15f, transY + 0.65f, pz + 0.52f},
                Vec3{0.22f, transY + 0.98f, pz + 0.52f},
                0.045f, 0.028f, 10, ceramicMat, true, true);
        }
    }

    // 3. Continuous Bent Cylinder Power Cables strung between posts using Catenary Math
    float sagDepth = 1.15f;
    float cableRadius = 0.022f;

    // Connect consecutive posts (0->1 and 1->2)
    for (size_t p = 0; p < poleZ.size() - 1; ++p) {
        const auto& listA = postAttachments[p];
        const auto& listB = postAttachments[p + 1];
        size_t count = std::min(listA.size(), listB.size());

        for (size_t i = 0; i < count; ++i) {
            Vec3 p0 = listA[i].pos;
            Vec3 p1 = listB[i].pos;

            // Apex shield wire has slightly tighter tension (less sag)
            float wireSag = (i == count - 1) ? (sagDepth * 0.62f) : sagDepth;

            BuildingComponents::appendBentCylinderCatenary(
                outVerts, outIndices,
                p0, p1,
                wireSag,
                cableRadius,
                28,
                10,
                cableMat
            );
        }
    }

    // Lead-in and lead-out catenary extensions beyond the corridor ends
    for (const auto& att : postAttachments.front()) {
        Vec3 pEnd = att.pos;
        Vec3 pFar{pEnd.x, pEnd.y + 0.35f, -halfLen};
        BuildingComponents::appendBentCylinderCatenary(
            outVerts, outIndices,
            pFar, pEnd,
            sagDepth * 0.45f,
            cableRadius,
            16,
            8,
            cableMat
        );
    }
    for (const auto& att : postAttachments.back()) {
        Vec3 pStart = att.pos;
        Vec3 pFar{pStart.x, pStart.y + 0.35f, halfLen};
        BuildingComponents::appendBentCylinderCatenary(
            outVerts, outIndices,
            pStart, pFar,
            sagDepth * 0.45f,
            cableRadius,
            16,
            8,
            cableMat
        );
    }
}

} // namespace procedural::buildings
