// ============================================================================
// PalmAndMonocotBuilder.cpp
// Monocaul/Doum Palms, Giant Herbaceous Banana (Musa) & Pawpaw (Carica papaya)
// ============================================================================

#include "TreeCategoryBuilders.h"
#include <cmath>
#include <algorithm>

namespace vegetation {
namespace categories {

void buildPalmArchitecture(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
) {
    CrownControlParams hull = computeSpeciesCrownHull(ctx);
    const bool isDoum = (ctx.speciesId == TreeSpeciesId::DOUM_PALM);
    const bool isCoconut = (ctx.speciesId == TreeSpeciesId::COCONUT_PALM);
    int trunkSteps = 10;
    float stepH = hull.trunkHeight / static_cast<float>(trunkSteps);
    Vec3 currPos(0.0f, 0.0f, 0.0f);
    float currR = ctx.trunkRadius;

    // Coastal Coconut Palms naturally lean with a graceful phototropic bow toward sea light
    float leanAz = rng.nextFloat01() * 6.28318f;
    float leanMag = isCoconut ? 0.14f : 0.03f;

    for (int t = 0; t < trunkSteps; ++t) {
        float frac = static_cast<float>(t) / static_cast<float>(trunkSteps);
        float curveLat = leanMag * stepH * std::sin(frac * 2.2f);
        float curveX = std::cos(leanAz) * curveLat + rng.nextRange(-0.015f, 0.015f) * stepH;
        float curveZ = std::sin(leanAz) * curveLat + rng.nextRange(-0.015f, 0.015f) * stepH;
        Vec3 nextPos = currPos + Vec3(curveX, stepH, curveZ);
        float nextR = currR * (t == 0 ? 0.90f : 0.975f);

        TreeBranchSegment seg;
        seg.startPos = currPos;
        seg.endPos = nextPos;
        seg.radiusStart = currR;
        seg.radiusEnd = nextR;
        seg.recursionDepth = 0;
        seg.branchIndex = static_cast<int>(outBranches.size());
        seg.windWeight = (nextPos.y / ctx.treeHeight) * 0.25f;
        seg.normal = (nextPos - currPos).normalized();

        outBranches.push_back(seg);
        currPos = nextPos;
        currR = nextR;
    }

    // Doum Palm: distinctive dichotomous Y-bifurcation at upper trunk
    std::vector<Vec3> crownCenters = { currPos };
    if (isDoum) {
        crownCenters.clear();
        for (int fork = 0; fork < 2; ++fork) {
            float forkAz = leanAz + (fork == 0 ? 0.0f : 3.14159f);
            Vec3 forkDir = Vec3(std::cos(forkAz) * 0.44f, 0.88f, std::sin(forkAz) * 0.44f).normalized();
            Vec3 forkEnd = currPos + forkDir * (ctx.treeHeight * 0.18f);

            TreeBranchSegment fSeg;
            fSeg.startPos = currPos;
            fSeg.endPos = forkEnd;
            fSeg.radiusStart = currR;
            fSeg.radiusEnd = currR * 0.74f;
            fSeg.recursionDepth = 0; // Stipe fork remains Order 0 woody stem before frond crown
            fSeg.branchIndex = static_cast<int>(outBranches.size());
            fSeg.windWeight = 0.50f;
            fSeg.normal = forkDir;

            outBranches.push_back(fSeg);
            crownCenters.push_back(forkEnd);
        }
    }

    int frondsPerCenter = isDoum
        ? std::clamp(ctx.branchNumber / static_cast<int>(crownCenters.size()), 10, 15)
        : std::clamp(ctx.branchNumber, 18, 24);
    for (const auto& center : crownCenters) {
        float baseAz = rng.nextFloat01() * 6.28318f;
        for (int f = 0; f < frondsPerCenter; ++f) {
            float az = baseAz + static_cast<float>(f) * TreeTypeBase::GOLDEN_ANGLE_RAD;
            float frondLen = (ctx.treeHeight * 0.28f) * rng.nextRange(0.88f, 1.12f);

            float frac = static_cast<float>(f) / static_cast<float>(frondsPerCenter);
            float sheathY = (0.5f - frac) * (ctx.treeHeight * 0.04f);
            Vec3 rStart = center + Vec3(0.0f, sheathY, 0.0f);
            float rachisR = currR * 0.26f;

            float basePitchDeg = 24.0f + frac * 54.0f + rng.nextRange(-4.0f, 4.0f);
            int rachisSegments = 4;
            float rSegLen = frondLen / static_cast<float>(rachisSegments);
            Vec3 distalDir(0.0f, 1.0f, 0.0f);
            int distalSegIdx = -1;

            for (int s = 0; s < rachisSegments; ++s) {
                float sFrac = static_cast<float>(s) / static_cast<float>(rachisSegments);
                float pitchDeg = basePitchDeg + sFrac * 42.0f;
                float pitchRad = pitchDeg * (3.14159265f / 180.0f);

                Vec3 rDir(
                    std::sin(pitchRad) * std::cos(az),
                    std::cos(pitchRad) * 0.65f - sFrac * 0.50f,
                    std::sin(pitchRad) * std::sin(az)
                );
                rDir = rDir.normalized();

                Vec3 rEnd = rStart + rDir * rSegLen;
                float endR = (s == rachisSegments - 1)
                    ? (isDoum ? std::max(0.014f, currR * 0.06f) : 0.008f)
                    : (rachisR * (1.0f - sFrac * 0.68f));

                TreeBranchSegment bSeg;
                bSeg.startPos = rStart;
                bSeg.endPos = rEnd;
                bSeg.radiusStart = rachisR;
                bSeg.radiusEnd = endR;
                bSeg.recursionDepth = 1;
                bSeg.branchIndex = static_cast<int>(outBranches.size());
                bSeg.windWeight = std::min(1.0f, (rEnd.y / ctx.treeHeight) * 0.85f);
                bSeg.normal = rDir;

                distalSegIdx = bSeg.branchIndex;
                distalDir = rDir;
                outBranches.push_back(bSeg);
                rStart = rEnd;
                rachisR = endR;
            }

            if (isDoum && distalSegIdx >= 0) {
                TreeLeafData fan;
                fan.position = rStart - distalDir * 0.03f;
                fan.normal = (distalDir + Vec3(0.0f, -0.15f - frac * 0.25f, 0.0f)).normalized();
                fan.tangent = Vec3(-std::sin(az), 0.0f, std::cos(az)).normalized();
                fan.size = ctx.treeHeight * rng.nextRange(0.13f, 0.17f) * ctx.leafScale;
                fan.colorR = ctx.leafColorR * rng.nextRange(0.90f, 1.10f);
                fan.colorG = ctx.leafColorG * rng.nextRange(0.90f, 1.10f);
                fan.colorB = ctx.leafColorB * rng.nextRange(0.90f, 1.10f);
                fan.windWeight = std::clamp(rStart.y / ctx.treeHeight, 0.35f, 1.0f);
                fan.branchIndex = static_cast<uint32_t>(distalSegIdx);
                outLeaves.push_back(fan);
            }
        }
    }

    if (!isDoum) {
        int pinnaeCount = std::max(ctx.leafCount, static_cast<int>(crownCenters.size()) * frondsPerCenter * 12);
        TreeTypeBase::generateStojanovicLeaves(
            outBranches, outLeaves, rng,
            ctx.leafScale, ctx.leafColorR, ctx.leafColorG, ctx.leafColorB,
            ctx.treeHeight, pinnaeCount, true, 1, 2, ctx.realLeafLengthMeters
        );
    }
}

void buildBananaPlant(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
) {
    float stemHeight = ctx.treeHeight * 0.56f;
    int stemSteps = 8;
    float stepH = stemHeight / static_cast<float>(stemSteps);
    Vec3 currPos(0.0f, 0.0f, 0.0f);
    float currR = ctx.trunkRadius * 1.12f;

    struct StemCollarNode {
        Vec3 pos;
        float radius;
        float tierFrac;
    };
    std::vector<StemCollarNode> collarNodes;

    int firstCollarStep = stemSteps - 3;
    for (int t = 0; t < stemSteps; ++t) {
        float driftX = rng.nextRange(-0.012f, 0.012f) * stepH;
        float driftZ = rng.nextRange(-0.012f, 0.012f) * stepH;
        Vec3 nextPos = currPos + Vec3(driftX, stepH, driftZ);
        float taper = (t == 0) ? 0.88f : 0.94f;
        float nextR = std::max(ctx.trunkRadius * 0.62f, currR * taper);

        TreeBranchSegment seg;
        seg.startPos = currPos;
        seg.endPos = nextPos;
        seg.radiusStart = currR;
        seg.radiusEnd = nextR;
        seg.recursionDepth = 0;
        seg.branchIndex = static_cast<int>(outBranches.size());
        seg.windWeight = (nextPos.y / ctx.treeHeight) * 0.22f;
        seg.normal = (nextPos - currPos).normalized();
        outBranches.push_back(seg);

        if (t >= firstCollarStep) {
            float tierFrac = 1.0f - static_cast<float>(t - firstCollarStep) /
                static_cast<float>(std::max(1, (stemSteps - 1) - firstCollarStep));
            collarNodes.push_back({nextPos, nextR, tierFrac});
        }
        currPos = nextPos;
        currR = nextR;
    }

    int apexHeartIdx = static_cast<int>(outBranches.size());
    {
        float heartH = ctx.treeHeight * 0.055f;
        Vec3 heartEnd = currPos + Vec3(0.0f, heartH, 0.0f);
        TreeBranchSegment hSeg;
        hSeg.startPos = currPos;
        hSeg.endPos = heartEnd;
        hSeg.radiusStart = currR * 0.86f;
        hSeg.radiusEnd = currR * 0.36f;
        hSeg.recursionDepth = 0;
        hSeg.branchIndex = apexHeartIdx;
        hSeg.windWeight = (heartEnd.y / ctx.treeHeight) * 0.28f;
        hSeg.normal = Vec3(0.0f, 1.0f, 0.0f);
        outBranches.push_back(hSeg);
        currPos = heartEnd;
    }

    int petioleCount = std::clamp(ctx.branchNumber, 11, 15);
    float baseAz = rng.nextFloat01() * 6.28318f;
    size_t numNodes = collarNodes.size();

    for (int p = 0; p < petioleCount; ++p) {
        float crownFrac = static_cast<float>(p) / static_cast<float>(std::max(1, petioleCount - 1));
        size_t nodeIdx = (numNodes > 0)
            ? std::min(numNodes - 1, static_cast<size_t>((1.0f - crownFrac * 0.95f) * static_cast<float>(numNodes - 1) + 0.5f))
            : 0;
        const auto& node = (numNodes > 0) ? collarNodes[nodeIdx] : StemCollarNode{currPos, currR, crownFrac};

        float az = baseAz + static_cast<float>(p) * TreeTypeBase::GOLDEN_ANGLE_RAD + rng.nextRange(-0.07f, 0.07f);
        float petioleLen = ctx.treeHeight * rng.nextRange(0.11f, 0.16f);
        float basePitchDeg = 16.0f + crownFrac * 52.0f + rng.nextRange(-2.5f, 2.5f);
        int petioleSegments = 3;
        float segLen = petioleLen / static_cast<float>(petioleSegments);

        Vec3 pStart = node.pos;
        float segR = node.radius * 0.28f;
        float minTipR = std::max(0.015f, node.radius * 0.10f);
        Vec3 distalDir(0.0f, 1.0f, 0.0f);
        int distalSegIdx = -1;

        for (int s = 0; s < petioleSegments; ++s) {
            float sFrac = static_cast<float>(s + 1) / static_cast<float>(petioleSegments);
            float archDeg = basePitchDeg + sFrac * (18.0f + 16.0f * crownFrac);
            float archRad = archDeg * (3.14159265f / 180.0f);

            Vec3 segDir(
                std::sin(archRad) * std::cos(az),
                std::cos(archRad) - sFrac * 0.10f * crownFrac,
                std::sin(archRad) * std::sin(az)
            );
            segDir = segDir.normalized();

            Vec3 pEnd = pStart + segDir * segLen;
            float nextSegR = std::max(minTipR, segR * 0.74f);

            TreeBranchSegment pSeg;
            pSeg.startPos = pStart;
            pSeg.endPos = pEnd;
            pSeg.radiusStart = segR;
            pSeg.radiusEnd = nextSegR;
            pSeg.recursionDepth = 1;
            pSeg.branchIndex = static_cast<int>(outBranches.size());
            pSeg.windWeight = std::clamp(0.45f + 0.45f * sFrac, 0.35f, 0.95f);
            pSeg.normal = segDir;

            distalSegIdx = pSeg.branchIndex;
            distalDir = segDir;
            outBranches.push_back(pSeg);
            pStart = pEnd;
            segR = nextSegR;
        }

        if (distalSegIdx >= 0) {
            Vec3 tangVec(-std::sin(az), 0.0f, std::cos(az));
            Vec3 bladeDir = (distalDir * 0.94f + Vec3(0.0f, -0.05f - 0.14f * crownFrac, 0.0f)).normalized();
            float tierLenScale = 0.82f + 0.28f * std::sin(3.14159265f * std::clamp(0.18f + crownFrac * 0.72f, 0.0f, 1.0f));

            TreeLeafData blade;
            blade.position = pStart;
            blade.normal = bladeDir;
            blade.tangent = tangVec;
            blade.size = ctx.treeHeight * tierLenScale * rng.nextRange(0.32f, 0.38f) * ctx.leafScale;
            blade.colorR = ctx.leafColorR * rng.nextRange(0.92f, 1.08f);
            blade.colorG = ctx.leafColorG * rng.nextRange(0.94f, 1.08f);
            blade.colorB = ctx.leafColorB * rng.nextRange(0.90f, 1.06f);
            blade.windWeight = std::clamp(pStart.y / ctx.treeHeight, 0.4f, 1.0f);
            blade.branchIndex = static_cast<uint32_t>(distalSegIdx);
            outLeaves.push_back(blade);
        }
    }

    // Authentic tightly furled central banana "cigar leaf" spear rising vertically from crown heart
    float cigarAz = baseAz + 1.1f;
    float cigarTotalH = ctx.treeHeight * 0.24f;
    Vec3 cPos = currPos - Vec3(0.0f, 0.05f, 0.0f);
    float cRad = currR * 0.26f;
    constexpr int kCigarSegs = 3;
    for (int cs = 0; cs < kCigarSegs; ++cs) {
        float cFrac = static_cast<float>(cs + 1) / static_cast<float>(kCigarSegs);
        Vec3 cDir = Vec3(
            std::cos(cigarAz) * (0.04f + 0.08f * cFrac),
            1.0f,
            std::sin(cigarAz) * (0.04f + 0.08f * cFrac)
        ).normalized();
        Vec3 cNext = cPos + cDir * (cigarTotalH / static_cast<float>(kCigarSegs));
        float cNextRad = std::max(0.006f, currR * 0.26f * (1.0f - 0.85f * cFrac * cFrac));

        TreeBranchSegment cSeg;
        cSeg.startPos = cPos;
        cSeg.endPos = cNext;
        cSeg.radiusStart = cRad;
        cSeg.radiusEnd = cNextRad;
        cSeg.recursionDepth = 1;
        cSeg.branchIndex = static_cast<int>(outBranches.size());
        cSeg.windWeight = 0.55f + 0.30f * cFrac;
        cSeg.normal = cDir;
        outBranches.push_back(cSeg);

        cPos = cNext;
        cRad = cNextRad;
    }
}

void buildPapayaTree(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
) {
    // Pawpaw / Papaya (Carica papaya):
    // Straight slender unbranched columnar trunk rising to ~72% height,
    // packed fruit-bearing collar along upper trunk (48%..68% H) right beneath the umbrella
    // of 14-18 long horizontal-arching hollow petioles topped by 7-lobed palmate star blades.
    float stemHeight = ctx.treeHeight * 0.72f;
    int stemSteps = 10;
    float stepH = stemHeight / static_cast<float>(stemSteps);
    Vec3 currPos(0.0f, 0.0f, 0.0f);
    float currR = ctx.trunkRadius;

    struct StemCollarNode {
        Vec3 pos;
        float radius;
        float tierFrac;
    };
    std::vector<StemCollarNode> collarNodes;

    // Petioles attach only at the very top apical rosette (~64%..72% height),
    // leaving a prominent upper-trunk fruit zone directly underneath (48%..64% H)!
    int firstCollarStep = stemSteps - 3;
    for (int t = 0; t < stemSteps; ++t) {
        float driftX = rng.nextRange(-0.010f, 0.010f) * stepH;
        float driftZ = rng.nextRange(-0.010f, 0.010f) * stepH;
        Vec3 nextPos = currPos + Vec3(driftX, stepH, driftZ);
        float nextR = std::max(ctx.trunkRadius * 0.50f, currR * (t == 0 ? 0.90f : 0.95f));

        TreeBranchSegment seg;
        seg.startPos = currPos;
        seg.endPos = nextPos;
        seg.radiusStart = currR;
        seg.radiusEnd = nextR;
        seg.recursionDepth = 0;
        seg.branchIndex = static_cast<int>(outBranches.size());
        seg.windWeight = (nextPos.y / ctx.treeHeight) * 0.22f;
        seg.normal = (nextPos - currPos).normalized();
        outBranches.push_back(seg);

        if (t >= firstCollarStep) {
            float tierFrac = 1.0f - static_cast<float>(t - firstCollarStep) /
                static_cast<float>(std::max(1, (stemSteps - 1) - firstCollarStep));
            collarNodes.push_back({nextPos, nextR, tierFrac});
        }

        currPos = nextPos;
        currR = nextR;
    }

    int apexHeartIdx = static_cast<int>(outBranches.size());
    {
        float heartH = ctx.treeHeight * 0.05f;
        Vec3 heartEnd = currPos + Vec3(0.0f, heartH, 0.0f);
        TreeBranchSegment hSeg;
        hSeg.startPos = currPos;
        hSeg.endPos = heartEnd;
        hSeg.radiusStart = currR * 0.85f;
        hSeg.radiusEnd = currR * 0.26f;
        hSeg.recursionDepth = 0;
        hSeg.branchIndex = apexHeartIdx;
        hSeg.windWeight = (heartEnd.y / ctx.treeHeight) * 0.28f;
        hSeg.normal = Vec3(0.0f, 1.0f, 0.0f);
        outBranches.push_back(hSeg);
        currPos = heartEnd;
    }

    int petioleCount = std::clamp(ctx.branchNumber, 13, 18);
    float baseAz = rng.nextFloat01() * 6.28318f;
    size_t numNodes = collarNodes.size();

    for (int p = 0; p < petioleCount; ++p) {
        float crownFrac = static_cast<float>(p) / static_cast<float>(std::max(1, petioleCount - 1));
        size_t nodeIdx = (numNodes > 0)
            ? std::min(numNodes - 1, static_cast<size_t>((1.0f - crownFrac * 0.95f) * static_cast<float>(numNodes - 1) + 0.5f))
            : 0;
        const auto& node = (numNodes > 0) ? collarNodes[nodeIdx] : StemCollarNode{currPos, currR, crownFrac};

        float az = baseAz + static_cast<float>(p) * TreeTypeBase::GOLDEN_ANGLE_RAD + rng.nextRange(-0.07f, 0.07f);
        float petioleLen = ctx.treeHeight * rng.nextRange(0.24f, 0.31f);
        // Young upper petioles ascend steeply (~34 deg), mature lower petioles radiate nearly horizontally (~82 deg)
        float basePitchDeg = 34.0f + crownFrac * 48.0f + rng.nextRange(-2.5f, 2.5f);
        int petioleSegments = 2;
        float segLen = petioleLen / static_cast<float>(petioleSegments);

        Vec3 pStart = node.pos;
        float segR = node.radius * 0.22f;
        float minTipR = std::max(0.010f, node.radius * 0.08f);
        Vec3 distalDir(0.0f, 1.0f, 0.0f);
        int distalSegIdx = -1;

        for (int s = 0; s < petioleSegments; ++s) {
            float sFrac = static_cast<float>(s + 1) / static_cast<float>(petioleSegments);
            float archDeg = basePitchDeg + sFrac * 14.0f * (0.4f + crownFrac);
            float archRad = archDeg * (3.14159265f / 180.0f);

            Vec3 segDir(
                std::sin(archRad) * std::cos(az),
                std::cos(archRad) - sFrac * 0.10f * crownFrac,
                std::sin(archRad) * std::sin(az)
            );
            segDir = segDir.normalized();

            Vec3 pEnd = pStart + segDir * segLen;
            float nextSegR = std::max(minTipR, segR * 0.68f);

            TreeBranchSegment pSeg;
            pSeg.startPos = pStart;
            pSeg.endPos = pEnd;
            pSeg.radiusStart = segR;
            pSeg.radiusEnd = nextSegR;
            pSeg.recursionDepth = 1;
            pSeg.branchIndex = static_cast<int>(outBranches.size());
            pSeg.windWeight = std::clamp(0.45f + 0.45f * sFrac, 0.35f, 0.95f);
            pSeg.normal = segDir;

            distalSegIdx = pSeg.branchIndex;
            distalDir = segDir;
            outBranches.push_back(pSeg);
            pStart = pEnd;
            segR = nextSegR;
        }

        if (distalSegIdx >= 0) {
            Vec3 tangVec(-std::sin(az), 0.0f, std::cos(az));
            Vec3 bladeDir = (distalDir * 0.68f + Vec3(0.0f, -0.22f - 0.35f * crownFrac, 0.0f)).normalized();
            TreeLeafData starLeaf;
            starLeaf.position = pStart - distalDir * 0.02f;
            starLeaf.normal = bladeDir;
            starLeaf.tangent = tangVec;
            starLeaf.size = ctx.treeHeight * rng.nextRange(0.14f, 0.18f) * ctx.leafScale;
            starLeaf.colorR = ctx.leafColorR * rng.nextRange(0.90f, 1.12f);
            starLeaf.colorG = ctx.leafColorG * rng.nextRange(0.90f, 1.10f);
            starLeaf.colorB = ctx.leafColorB * rng.nextRange(0.88f, 1.10f);
            starLeaf.windWeight = std::clamp(pStart.y / ctx.treeHeight, 0.45f, 1.0f);
            starLeaf.branchIndex = static_cast<uint32_t>(distalSegIdx);
            outLeaves.push_back(starLeaf);
        }
    }

    // Young central apex flush shoots
    for (int c = 0; c < 2; ++c) {
        float cAz = baseAz + static_cast<float>(c) * 2.4f;
        Vec3 cDir(std::cos(cAz) * 0.22f, 0.96f, std::sin(cAz) * 0.22f);
        TreeLeafData apexLeaf;
        apexLeaf.position = currPos - Vec3(0.0f, 0.04f, 0.0f);
        apexLeaf.normal = cDir.normalized();
        apexLeaf.tangent = Vec3(-std::sin(cAz), 0.0f, std::cos(cAz));
        apexLeaf.size = ctx.treeHeight * 0.11f * ctx.leafScale;
        apexLeaf.colorR = std::min(1.0f, ctx.leafColorR * 1.15f);
        apexLeaf.colorG = std::min(1.0f, ctx.leafColorG * 1.18f);
        apexLeaf.colorB = ctx.leafColorB * 0.95f;
        apexLeaf.windWeight = 0.65f;
        apexLeaf.branchIndex = static_cast<uint32_t>(apexHeartIdx);
        outLeaves.push_back(apexLeaf);
    }
}

} // namespace categories
} // namespace vegetation
