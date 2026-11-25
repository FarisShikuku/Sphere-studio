// ============================================================================
// ConiferAndSpireBuilder.cpp
// Excurrent Whorled Conifers & Fastigiate Columnar Cypress
// ============================================================================

#include "TreeCategoryBuilders.h"
#include <cmath>
#include <algorithm>

namespace vegetation {
namespace categories {

void buildConiferSpire(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
) {
    int leaderSteps = 12;
    float stepH = ctx.treeHeight / static_cast<float>(leaderSteps);
    Vec3 currPos(0.0f, 0.0f, 0.0f);
    float currR = ctx.trunkRadius;

    // Mature tall forest conifers self-prune lower shaded limbs to a tall clean bole
    bool tallForestConifer = (ctx.speciesId == TreeSpeciesId::GIANT_REDWOOD ||
                              ctx.speciesId == TreeSpeciesId::GIANT_SEQUOIA ||
                              ctx.speciesId == TreeSpeciesId::SCOTS_PINE ||
                              ctx.speciesId == TreeSpeciesId::BEACH_CASUARINA);
    int firstWhorlStep = ctx.isFastigiateCypress ? 2 : (tallForestConifer ? 5 : 3);
    std::vector<Vec3> tierNodes;

    for (int t = 0; t < leaderSteps; ++t) {
        Vec3 nextPos = currPos + Vec3(0.0f, stepH, 0.0f);
        float nextR = (t == leaderSteps - 1) ? 0.006f : (currR * (t < firstWhorlStep ? 0.94f : 0.87f));

        TreeBranchSegment seg;
        seg.startPos = currPos;
        seg.endPos = nextPos;
        seg.radiusStart = currR;
        seg.radiusEnd = nextR;
        seg.recursionDepth = 0;
        seg.branchIndex = static_cast<int>(outBranches.size());
        seg.windWeight = (nextPos.y / ctx.treeHeight) * 0.25f;
        seg.normal = Vec3(0.0f, 1.0f, 0.0f);

        outBranches.push_back(seg);
        if (t >= firstWhorlStep && t < leaderSteps - 1) tierNodes.push_back(nextPos);
        currPos = nextPos;
        currR = nextR;
    }

    // Sub-apical terminal shoots clothing the conifer apical spire (Order 2 so they bear needles)
    Vec3 spireTip = currPos;
    for (int st = 0; st < 3; ++st) {
        float stAz = static_cast<float>(st) * (6.28318f / 3.0f) + rng.nextRange(-0.1f, 0.1f);
        Vec3 stDir = Vec3(std::cos(stAz) * 0.28f, 0.95f, std::sin(stAz) * 0.28f).normalized();
        Vec3 stEnd = spireTip + stDir * (stepH * 0.55f);

        TreeBranchSegment stSeg;
        stSeg.startPos = spireTip;
        stSeg.endPos = stEnd;
        stSeg.radiusStart = 0.008f;
        stSeg.radiusEnd = 0.003f;
        stSeg.recursionDepth = 2;
        stSeg.branchIndex = static_cast<int>(outBranches.size());
        stSeg.windWeight = 0.95f;
        stSeg.normal = stDir;
        outBranches.push_back(stSeg);
    }

    bool isCedar = (ctx.speciesId == TreeSpeciesId::ATLAS_CEDAR);
    bool isCypress = ctx.isFastigiateCypress;
    int maxConiferSegments = std::max(640, ctx.branchNumber * 14);

    struct BoughBranchPoint {
        Vec3 pos;
        Vec3 dir;
        Vec3 perpBough;
        float radius;
        float tierSpread;
        float tierFrac;
    };
    std::vector<BoughBranchPoint> allLateralNodes;

    for (size_t tierIdx = 0; tierIdx < tierNodes.size(); ++tierIdx) {
        if (outBranches.size() >= static_cast<size_t>(maxConiferSegments)) break;

        Vec3 nodePos = tierNodes[tierIdx];
        float tierFrac = static_cast<float>(tierIdx) / static_cast<float>(std::max<size_t>(1, tierNodes.size()));
        float tierSpread = isCypress
            ? (1.0f - tierFrac * 0.55f) * (ctx.treeHeight * 0.085f)
            : (1.0f - tierFrac * 0.68f) * (ctx.treeHeight * 0.27f);

        int boughsInWhorl = isCypress ? 5 : 4;
        for (int b = 0; b < boughsInWhorl; ++b) {
            if (outBranches.size() >= static_cast<size_t>(maxConiferSegments)) break;

            float az = static_cast<float>(b) * (6.28318f / static_cast<float>(boughsInWhorl))
                     + static_cast<float>(tierIdx) * 0.42f + rng.nextRange(-0.10f, 0.10f);

            float bStartR = std::min(0.22f, ctx.trunkRadius * 0.22f) * (1.0f - tierFrac * 0.68f);
            bStartR = std::max(isCypress ? 0.016f : 0.024f, bStartR);

            int bSteps = 3;
            Vec3 segStart = nodePos;
            float segR = bStartR;
            std::vector<BoughBranchPoint> boughLaterals;
            Vec3 finalDir(std::cos(az), isCypress ? 2.4f : 0.15f, std::sin(az));

            for (int s = 0; s < bSteps; ++s) {
                float sFrac0 = static_cast<float>(s) / static_cast<float>(bSteps);
                float sFrac1 = static_cast<float>(s + 1) / static_cast<float>(bSteps);

                float yLift = isCypress
                    ? (1.65f + 0.95f * sFrac1 + 0.40f * tierFrac)
                    : (isCedar
                        ? (0.14f + 0.08f * tierFrac)
                        : ((-0.14f * (1.0f - tierFrac) + 0.36f * sFrac0) + 0.18f * tierFrac));

                Vec3 stepDir = Vec3(std::cos(az), yLift, std::sin(az)).normalized();
                float stepLen = (tierSpread / static_cast<float>(bSteps)) * rng.nextRange(0.92f, 1.08f);
                Vec3 segEnd = segStart + stepDir * stepLen;

                float nextSegR = (s == bSteps - 1)
                    ? std::min(0.015f, bStartR * 0.22f)
                    : bStartR * (1.0f - sFrac1 * 0.65f);

                TreeBranchSegment bSeg;
                bSeg.startPos = segStart;
                bSeg.endPos = segEnd;
                bSeg.radiusStart = segR;
                bSeg.radiusEnd = nextSegR;
                bSeg.recursionDepth = 1;
                bSeg.branchIndex = static_cast<int>(outBranches.size());
                bSeg.windWeight = std::min(1.0f, (segEnd.y / ctx.treeHeight) * (0.45f + 0.35f * sFrac1));
                bSeg.normal = stepDir;
                outBranches.push_back(bSeg);

                if (s == 0 && tierFrac < 0.72f) {
                    boughLaterals.push_back({segEnd, stepDir, Vec3(0.0f, 0.0f, 0.0f), nextSegR, tierSpread * 0.78f, tierFrac});
                } else if (s == 1) {
                    boughLaterals.push_back({segEnd, stepDir, Vec3(0.0f, 0.0f, 0.0f), nextSegR, tierSpread, tierFrac});
                }

                segStart = segEnd;
                segR = nextSegR;
                finalDir = stepDir;
            }

            Vec3 boughTip = segStart;
            float tipR = segR;

            Vec3 perpBough = cross(finalDir, Vec3(0.0f, 1.0f, 0.0f));
            if (perpBough.lengthSq() < 0.01f) perpBough = cross(finalDir, Vec3(1.0f, 0.0f, 0.0f));
            perpBough = perpBough.normalized();

            for (auto& ln : boughLaterals) {
                ln.perpBough = perpBough;
                allLateralNodes.push_back(ln);
            }

            // Distal terminal shoots emerging directly from bough tip
            for (int k = -1; k <= 1; ++k) {
                if (outBranches.size() >= static_cast<size_t>(maxConiferSegments)) break;
                float fanBias = static_cast<float>(k) * 0.48f;
                Vec3 termDir = (finalDir * 0.88f + perpBough * fanBias + Vec3(0.0f, 0.14f, 0.0f)).normalized();
                float termLen = tierSpread * (k == 0 ? 0.28f : 0.24f) * rng.nextRange(0.88f, 1.12f);
                Vec3 termTip = boughTip + termDir * termLen;

                TreeBranchSegment termSeg;
                termSeg.startPos = boughTip;
                termSeg.endPos = termTip;
                termSeg.radiusStart = tipR * (k == 0 ? 0.90f : 0.75f);
                termSeg.radiusEnd = 0.004f;
                termSeg.recursionDepth = 2;
                termSeg.branchIndex = static_cast<int>(outBranches.size());
                termSeg.windWeight = std::min(1.0f, (termTip.y / ctx.treeHeight) * 0.95f);
                termSeg.normal = termDir;
                outBranches.push_back(termSeg);
            }
        }
    }

    struct ConiferOrder2Tip {
        Vec3 pos;
        Vec3 dir;
        float radius;
        float tierSpread;
    };
    std::vector<ConiferOrder2Tip> order2Tips;

    // Pass 2a: Mid-bough & proximal Order 2 lateral sprays across all whorls
    for (const auto& ln : allLateralNodes) {
        if (outBranches.size() >= static_cast<size_t>(maxConiferSegments)) break;

        for (int sideSign = -1; sideSign <= 1; sideSign += 2) {
            if (outBranches.size() >= static_cast<size_t>(maxConiferSegments)) break;
            float droopOffset = (ctx.speciesId == TreeSpeciesId::NORWAY_SPRUCE) ? -0.10f : -0.04f;
            Vec3 twDir = (ln.dir * 0.72f + ln.perpBough * (0.62f * static_cast<float>(sideSign)) + Vec3(0.0f, droopOffset, 0.0f)).normalized();
            float twLen = ln.tierSpread * 0.34f * rng.nextRange(0.85f, 1.15f);
            Vec3 twTip = ln.pos + twDir * twLen;
            float twStartR = ln.radius * 0.70f;
            float twEndR = std::min(0.008f, twStartR * 0.45f);

            TreeBranchSegment twSeg;
            twSeg.startPos = ln.pos;
            twSeg.endPos = twTip;
            twSeg.radiusStart = twStartR;
            twSeg.radiusEnd = twEndR;
            twSeg.recursionDepth = 2;
            twSeg.branchIndex = static_cast<int>(outBranches.size());
            twSeg.windWeight = std::min(1.0f, (twTip.y / ctx.treeHeight) * 0.88f);
            twSeg.normal = twDir;
            outBranches.push_back(twSeg);

            if (ln.tierFrac < 0.85f) {
                order2Tips.push_back({twTip, twDir, twEndR, ln.tierSpread});
            }
        }
    }

    // Pass 2b: Order 3 fine needle branchlets at outer mid-bough sprays
    for (const auto& o2 : order2Tips) {
        if (outBranches.size() >= static_cast<size_t>(maxConiferSegments)) break;

        Vec3 o2Perp = cross(o2.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (o2Perp.lengthSq() < 0.01f) o2Perp = cross(o2.dir, Vec3(1.0f, 0.0f, 0.0f));
        o2Perp = o2Perp.normalized();

        for (int s3 = -1; s3 <= 1; s3 += 2) {
            if (outBranches.size() >= static_cast<size_t>(maxConiferSegments)) break;
            Vec3 s3Dir = (o2.dir * 0.80f + o2Perp * (0.50f * static_cast<float>(s3)) + Vec3(0.0f, 0.06f, 0.0f)).normalized();
            float s3Len = o2.tierSpread * 0.18f * rng.nextRange(0.85f, 1.15f);
            Vec3 s3Tip = o2.pos + s3Dir * s3Len;

            TreeBranchSegment s3Seg;
            s3Seg.startPos = o2.pos;
            s3Seg.endPos = s3Tip;
            s3Seg.radiusStart = o2.radius * 0.85f;
            s3Seg.radiusEnd = 0.003f;
            s3Seg.recursionDepth = 3;
            s3Seg.branchIndex = static_cast<int>(outBranches.size());
            s3Seg.windWeight = std::min(1.0f, (s3Tip.y / ctx.treeHeight) * 0.98f);
            s3Seg.normal = s3Dir;
            outBranches.push_back(s3Seg);
        }
    }

    int needleCount = std::max(1450, ctx.leafCount);
    bool radialNeedles = (ctx.speciesId == TreeSpeciesId::NORWAY_SPRUCE);
    TreeTypeBase::generateConiferNeedles(
        outBranches, outLeaves, rng,
        ctx.leafScale, ctx.leafColorR, ctx.leafColorG, ctx.leafColorB,
        ctx.treeHeight, needleCount, radialNeedles
    );
}

} // namespace categories
} // namespace vegetation
