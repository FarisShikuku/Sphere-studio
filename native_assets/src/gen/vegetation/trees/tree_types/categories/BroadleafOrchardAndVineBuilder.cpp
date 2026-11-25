// ============================================================================
// BroadleafOrchardAndVineBuilder.cpp
// Decurrent Broadleaf, Tropical Rainforest Giants, Orchards, Multi-Stem Vase,
// Weeping Willow & Trunkless Climbing Liana Vine
// ============================================================================

#include "TreeCategoryBuilders.h"
#include <cmath>
#include <algorithm>

namespace vegetation {
namespace categories {

void buildWeepingPendulous(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
) {
    CrownControlParams hull = computeSpeciesCrownHull(ctx);
    int trunkSteps = 8;
    float stepH = hull.trunkHeight / static_cast<float>(trunkSteps);
    Vec3 currPos(0.0f, 0.0f, 0.0f);
    float currR = ctx.trunkRadius;

    struct WillowTrunkNode {
        Vec3 pos;
        float radius;
    };
    std::vector<WillowTrunkNode> trunkNodes;

    for (int t = 0; t < trunkSteps; ++t) {
        Vec3 nextPos = currPos + Vec3(rng.nextRange(-0.03f, 0.03f) * stepH, stepH, rng.nextRange(-0.03f, 0.03f) * stepH);
        float nextR = currR * 0.90f;

        TreeBranchSegment seg;
        seg.startPos = currPos;
        seg.endPos = nextPos;
        seg.radiusStart = currR;
        seg.radiusEnd = nextR;
        seg.recursionDepth = 0;
        seg.branchIndex = static_cast<int>(outBranches.size());
        seg.windWeight = (nextPos.y / ctx.treeHeight) * 0.20f;
        seg.normal = Vec3(0.0f, 1.0f, 0.0f);
        outBranches.push_back(seg);

        if (t >= 4 && t < trunkSteps - 1) {
            trunkNodes.push_back({nextPos, nextR});
        }
        currPos = nextPos;
        currR = nextR;
    }

    int maxAllowedBranches = std::max(180, ctx.branchNumber * 5);
    int scaffoldCount = 5;
    size_t numNodes = trunkNodes.size();

    struct WillowBoughAnchor {
        Vec3 pos;
        Vec3 dir;
        float radius;
    };
    std::vector<WillowBoughAnchor> boughAnchors;

    for (int sc = 0; sc < scaffoldCount; ++sc) {
        if (outBranches.size() >= static_cast<size_t>(maxAllowedBranches)) break;

        size_t nodeIdx = (numNodes > 0) ? (sc % numNodes) : 0;
        const auto& node = (numNodes > 0) ? trunkNodes[nodeIdx] : WillowTrunkNode{currPos, currR};

        float az = static_cast<float>(sc) * (6.28318f / static_cast<float>(scaffoldCount)) + rng.nextRange(-0.15f, 0.15f);
        Vec3 scDir = Vec3(std::cos(az) * 0.85f, 0.52f + rng.nextRange(-0.08f, 0.08f), std::sin(az) * 0.85f).normalized();
        float scLen = (ctx.treeHeight * 0.32f) * rng.nextRange(0.90f, 1.10f);
        Vec3 scEnd = node.pos + scDir * scLen;

        float startR = std::min(0.24f, node.radius * 0.45f);
        float endR = startR * TreeTypeBase::LEONARDO_RATIO;

        TreeBranchSegment scSeg;
        scSeg.startPos = node.pos;
        scSeg.endPos = scEnd;
        scSeg.radiusStart = startR;
        scSeg.radiusEnd = endR;
        scSeg.recursionDepth = 1;
        scSeg.branchIndex = static_cast<int>(outBranches.size());
        scSeg.windWeight = 0.55f;
        scSeg.normal = scDir;
        outBranches.push_back(scSeg);

        boughAnchors.push_back({node.pos + scDir * (scLen * 0.55f), scDir, endR});
        boughAnchors.push_back({scEnd, scDir, endR * TreeTypeBase::LEONARDO_RATIO});
    }

    struct StreamerBase {
        Vec3 pos;
        Vec3 dir;
        float radius;
    };
    std::vector<StreamerBase> streamers;

    for (size_t b = 0; b < boughAnchors.size(); ++b) {
        if (outBranches.size() >= static_cast<size_t>(maxAllowedBranches)) break;

        const auto& anchor = boughAnchors[b];
        for (int side = -1; side <= 1; side += 2) {
            if (outBranches.size() >= static_cast<size_t>(maxAllowedBranches)) break;

            Vec3 perp = cross(anchor.dir, Vec3(0.0f, 1.0f, 0.0f));
            if (perp.lengthSq() < 0.01f) perp = cross(anchor.dir, Vec3(1.0f, 0.0f, 0.0f));
            perp = perp.normalized();

            Vec3 boughDir = (anchor.dir * 0.70f + perp * (0.65f * static_cast<float>(side)) + Vec3(0.0f, 0.10f, 0.0f)).normalized();
            float boughLen = (ctx.treeHeight * 0.22f) * rng.nextRange(0.85f, 1.15f);
            Vec3 boughTip = anchor.pos + boughDir * boughLen;

            TreeBranchSegment bSeg;
            bSeg.startPos = anchor.pos;
            bSeg.endPos = boughTip;
            bSeg.radiusStart = anchor.radius;
            bSeg.radiusEnd = anchor.radius * TreeTypeBase::LEONARDO_RATIO;
            bSeg.recursionDepth = 2;
            bSeg.branchIndex = static_cast<int>(outBranches.size());
            bSeg.windWeight = 0.72f;
            bSeg.normal = boughDir;
            outBranches.push_back(bSeg);

            streamers.push_back({anchor.pos + boughDir * (boughLen * 0.50f), boughDir, bSeg.radiusEnd});
            streamers.push_back({boughTip, boughDir, 0.018f});
        }
    }

    for (size_t s = 0; s < streamers.size(); ++s) {
        if (outBranches.size() >= static_cast<size_t>(maxAllowedBranches)) break;

        const auto& st = streamers[s];
        float totalHangLen = (ctx.treeHeight * 0.42f) * rng.nextRange(0.85f, 1.15f);
        float seg1Len = totalHangLen * 0.50f;
        float seg2Len = totalHangLen * 0.50f;

        Vec3 st1Dir = Vec3(
            st.dir.x * 0.30f + rng.nextRange(-0.08f, 0.08f),
            -0.88f,
            st.dir.z * 0.30f + rng.nextRange(-0.08f, 0.08f)
        ).normalized();
        Vec3 st1End = st.pos + st1Dir * seg1Len;

        TreeBranchSegment st1Seg;
        st1Seg.startPos = st.pos;
        st1Seg.endPos = st1End;
        st1Seg.radiusStart = std::max(0.014f, st.radius);
        st1Seg.radiusEnd = 0.008f;
        st1Seg.recursionDepth = 3;
        st1Seg.branchIndex = static_cast<int>(outBranches.size());
        st1Seg.windWeight = 0.88f;
        st1Seg.normal = st1Dir;
        outBranches.push_back(st1Seg);

        if (outBranches.size() < static_cast<size_t>(maxAllowedBranches)) {
            Vec3 st2Dir = Vec3(
                rng.nextRange(-0.06f, 0.06f),
                -0.98f,
                rng.nextRange(-0.06f, 0.06f)
            ).normalized();
            Vec3 st2End = st1End + st2Dir * seg2Len;

            TreeBranchSegment st2Seg;
            st2Seg.startPos = st1End;
            st2Seg.endPos = st2End;
            st2Seg.radiusStart = 0.008f;
            st2Seg.radiusEnd = 0.004f;
            st2Seg.recursionDepth = 4;
            st2Seg.branchIndex = static_cast<int>(outBranches.size());
            st2Seg.windWeight = 0.98f;
            st2Seg.normal = st2Dir;
            outBranches.push_back(st2Seg);
        }
    }

    int leafCount = std::max(1450, ctx.leafCount);
    TreeTypeBase::generateStojanovicLeaves(
        outBranches, outLeaves, rng,
        ctx.leafScale, ctx.leafColorR, ctx.leafColorG, ctx.leafColorB,
        ctx.treeHeight, leafCount, false, 2, ctx.terminalLeafCount, ctx.realLeafLengthMeters
    );
}

void buildMultiStemOrnamental(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
) {
    const bool isJapaneseMaple = (ctx.speciesId == TreeSpeciesId::JAPANESE_MAPLE);
    const int stemCount = isJapaneseMaple ? 4 : 5;
    float baseH = ctx.treeHeight * 0.11f;
    float baseR = ctx.trunkRadius * 1.15f;

    Vec3 rootPos(0.0f, 0.0f, 0.0f);
    Vec3 collarPos(0.0f, baseH, 0.0f);
    {
        TreeBranchSegment collar;
        collar.startPos = rootPos;
        collar.endPos = collarPos;
        collar.radiusStart = baseR;
        collar.radiusEnd = baseR * 0.85f;
        collar.recursionDepth = 0;
        collar.branchIndex = static_cast<int>(outBranches.size());
        collar.windWeight = 0.04f;
        collar.normal = Vec3(0.0f, 1.0f, 0.0f);
        outBranches.push_back(collar);
    }

    struct OrnamentalTip {
        Vec3 pos;
        Vec3 dir;
        float radius;
        float len;
    };
    std::vector<OrnamentalTip> order2Tips;
    float baseAz = rng.nextFloat01() * 6.283185f;

    for (int st = 0; st < stemCount; ++st) {
        float az = baseAz + static_cast<float>(st) * (6.283185f / static_cast<float>(stemCount)) + rng.nextRange(-0.18f, 0.18f);
        float spreadOut = isJapaneseMaple ? 0.56f : 0.36f;
        Vec3 currPos = collarPos;
        float currR = baseR * (0.44f + rng.nextRange(-0.04f, 0.04f));
        float totalStemLen = ctx.treeHeight * rng.nextRange(0.54f, 0.64f);
        int steps = 5;
        float stepLen = totalStemLen / static_cast<float>(steps);

        Vec3 stemDir = Vec3(std::cos(az) * spreadOut, 0.88f, std::sin(az) * spreadOut).normalized();

        for (int s = 0; s < steps; ++s) {
            float t1 = static_cast<float>(s + 1) / static_cast<float>(steps);
            float radialCur = isJapaneseMaple ? (spreadOut + 0.35f * t1) : (spreadOut * (1.1f - 0.55f * t1));
            float pitchY = isJapaneseMaple ? (0.85f - 0.32f * t1) : (0.82f + 0.24f * t1);
            float nJitter = veg_noise3d(currPos.x * 0.9f, currPos.y * 0.9f + st * 4.0f, currPos.z * 0.9f) * 0.14f;
            stemDir = Vec3(std::cos(az + nJitter) * radialCur, pitchY, std::sin(az + nJitter) * radialCur).normalized();

            Vec3 nextPos = currPos + stemDir * stepLen;
            float nextR = currR * 0.84f;

            TreeBranchSegment seg;
            seg.startPos = currPos;
            seg.endPos = nextPos;
            seg.radiusStart = currR;
            seg.radiusEnd = nextR;
            seg.recursionDepth = 0;
            seg.branchIndex = static_cast<int>(outBranches.size());
            seg.windWeight = std::min(0.60f, (nextPos.y / ctx.treeHeight) * 0.38f);
            seg.normal = stemDir;
            outBranches.push_back(seg);

            if (s >= 2) {
                Vec3 side = cross(stemDir, Vec3(0.0f, 1.0f, 0.0f));
                if (side.lengthSq() < 0.01f) side = Vec3(1.0f, 0.0f, 0.0f);
                side = side.normalized();

                int sideCount = (s == steps - 1) ? 2 : 1;
                for (int k = 0; k < sideCount; ++k) {
                    float rot = ((s + k) % 2 == 0 ? 0.65f : -0.65f) + rng.nextRange(-0.18f, 0.18f);
                    Vec3 subDir = (stemDir * 0.72f + side * std::sin(rot) * 0.65f + Vec3(0.0f, isJapaneseMaple ? -0.18f : 0.22f, 0.0f)).normalized();
                    float subLen = ctx.treeHeight * (0.24f - t1 * 0.05f);
                    Vec3 subEnd = nextPos + subDir * subLen;
                    float subStartR = nextR * 0.64f;
                    float subEndR = std::max(0.012f, subStartR * 0.62f);

                    TreeBranchSegment bSeg;
                    bSeg.startPos = nextPos;
                    bSeg.endPos = subEnd;
                    bSeg.radiusStart = subStartR;
                    bSeg.radiusEnd = subEndR;
                    bSeg.recursionDepth = 1;
                    bSeg.branchIndex = static_cast<int>(outBranches.size());
                    bSeg.windWeight = std::min(0.85f, (subEnd.y / ctx.treeHeight) * 0.68f);
                    bSeg.normal = subDir;
                    outBranches.push_back(bSeg);

                    order2Tips.push_back({subEnd, subDir, subEndR, subLen});
                }
            }
            currPos = nextPos;
            currR = nextR;
        }
        order2Tips.push_back({currPos, stemDir, currR, stepLen});
    }

    std::vector<OrnamentalTip> order3Anchors;
    for (const auto& o2 : order2Tips) {
        Vec3 side = cross(o2.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (side.lengthSq() < 0.01f) side = cross(o2.dir, Vec3(1.0f, 0.0f, 0.0f));
        side = side.normalized();

        for (int f = 0; f < 2; ++f) {
            float sign = (f == 0) ? 1.0f : -1.0f;
            Vec3 fDir = (o2.dir * 0.80f + side * (0.50f * sign) + Vec3(0.0f, isJapaneseMaple ? -0.06f : 0.16f, 0.0f)).normalized();
            float fLen = o2.len * 0.56f * rng.nextRange(0.88f, 1.12f);
            Vec3 fEnd = o2.pos + fDir * fLen;
            float fEndR = std::max(0.008f, o2.radius * 0.60f);

            TreeBranchSegment seg;
            seg.startPos = o2.pos;
            seg.endPos = fEnd;
            seg.radiusStart = o2.radius * 0.82f;
            seg.radiusEnd = fEndR;
            seg.recursionDepth = 2;
            seg.branchIndex = static_cast<int>(outBranches.size());
            seg.windWeight = 0.88f;
            seg.normal = fDir;
            outBranches.push_back(seg);

            order3Anchors.push_back({fEnd, fDir, fEndR, fLen});
        }
    }

    for (const auto& o3 : order3Anchors) {
        Vec3 side = cross(o3.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (side.lengthSq() < 0.01f) side = cross(o3.dir, Vec3(1.0f, 0.0f, 0.0f));
        side = side.normalized();

        for (int f = 0; f < 3; ++f) {
            float sign = (f == 0) ? 1.0f : (f == 1 ? -1.0f : 0.0f);
            Vec3 fDir = (o3.dir * 0.84f + side * (0.44f * sign) + Vec3(0.0f, isJapaneseMaple ? -0.05f : (f == 2 ? 0.22f : 0.10f), 0.0f)).normalized();
            float fLen = o3.len * (f == 2 ? 0.44f : 0.54f);
            Vec3 fEnd = o3.pos + fDir * fLen;

            TreeBranchSegment seg;
            seg.startPos = o3.pos;
            seg.endPos = fEnd;
            seg.radiusStart = std::min(0.016f, o3.radius * 0.80f);
            seg.radiusEnd = 0.004f;
            seg.recursionDepth = 3;
            seg.branchIndex = static_cast<int>(outBranches.size());
            seg.windWeight = 0.96f;
            seg.normal = fDir;
            outBranches.push_back(seg);
        }
    }

    int leafCount = std::max(900, ctx.leafCount);
    TreeTypeBase::generateStojanovicLeaves(
        outBranches, outLeaves, rng,
        ctx.leafScale, ctx.leafColorR, ctx.leafColorG, ctx.leafColorB,
        ctx.treeHeight, leafCount, false, 2, ctx.terminalLeafCount, ctx.realLeafLengthMeters
    );
}

void buildClimbingVine(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
) {
    const float caneBaseR = std::clamp(ctx.trunkRadius, 0.012f, 0.028f);
    const int caneCount = std::clamp(8 + (ctx.branchNumber - 30) / 8, 7, 12);
    const float baseAz = rng.nextFloat01() * 6.283185f;

    struct VineNode {
        Vec3 pos;
        Vec3 dir;
        float radius;
        float len;
    };
    std::vector<VineNode> runnerAnchors;

    for (int c = 0; c < caneCount; ++c) {
        float az0 = baseAz + static_cast<float>(c) * (6.283185f / static_cast<float>(caneCount)) + rng.nextRange(-0.22f, 0.22f);
        float groundSpread = ctx.treeHeight * rng.nextRange(0.06f, 0.16f);
        Vec3 currPos(std::cos(az0) * groundSpread, 0.0f, std::sin(az0) * groundSpread);
        float currR = caneBaseR * rng.nextRange(0.84f, 1.05f);

        int caneSteps = 9;
        float caneTotalLen = ctx.treeHeight * rng.nextRange(1.05f, 1.38f);
        float stepLen = caneTotalLen / static_cast<float>(caneSteps);
        float spiralDir = (c % 2 == 0) ? 1.0f : -1.0f;
        float spiralSpeed = rng.nextRange(0.32f, 0.48f);
        float maxRadius = ctx.treeHeight * rng.nextRange(0.28f, 0.46f);

        for (int s = 0; s < caneSteps; ++s) {
            float t = static_cast<float>(s + 1) / static_cast<float>(caneSteps);
            float theta = az0 + spiralDir * t * 6.283185f * spiralSpeed * 1.6f;

            float radialTarget = (t < 0.60f)
                ? (groundSpread + (maxRadius * 0.62f) * std::sin(t * 2.2f))
                : (maxRadius * (0.62f + 0.48f * (t - 0.60f) / 0.40f));
            float yTarget = (t < 0.68f)
                ? (ctx.treeHeight * (t / 0.68f) * 0.92f)
                : (ctx.treeHeight * (0.92f - 0.34f * (t - 0.68f) / 0.32f));

            float nX = veg_noise3d(currPos.x * 1.3f + c, currPos.y * 1.1f, currPos.z * 1.3f) * 0.16f;
            float nZ = veg_noise3d(currPos.z * 1.3f, currPos.y * 1.1f + c * 3.0f, currPos.x * 1.3f) * 0.16f;
            Vec3 idealNext(
                std::cos(theta) * radialTarget + nX * ctx.treeHeight * 0.12f,
                std::max(0.18f, yTarget),
                std::sin(theta) * radialTarget + nZ * ctx.treeHeight * 0.12f
            );
            Vec3 segVec = idealNext - currPos;
            float segLen = segVec.length();
            if (segLen < 0.05f) {
                segVec = Vec3(std::cos(theta), 0.35f, std::sin(theta));
                segLen = stepLen;
            }
            Vec3 dir = segVec.normalized();
            Vec3 nextPos = currPos + dir * std::min(stepLen * 1.25f, std::max(stepLen * 0.72f, segLen));
            float nextR = std::max(0.005f, currR * 0.89f);

            TreeBranchSegment seg;
            seg.startPos = currPos;
            seg.endPos = nextPos;
            seg.radiusStart = currR;
            seg.radiusEnd = nextR;
            seg.recursionDepth = 1;
            seg.branchIndex = static_cast<int>(outBranches.size());
            seg.windWeight = std::min(0.88f, 0.12f + t * 0.76f);
            seg.normal = dir;
            outBranches.push_back(seg);

            if (s >= 2) runnerAnchors.push_back({nextPos, dir, nextR, stepLen});
            currPos = nextPos;
            currR = nextR;
        }
    }

    std::vector<VineNode> tendrilAnchors;
    for (size_t i = 0; i < runnerAnchors.size(); ++i) {
        const auto& anc = runnerAnchors[i];
        Vec3 side = cross(anc.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (side.lengthSq() < 0.01f) side = Vec3(1.0f, 0.0f, 0.0f);
        side = side.normalized();

        float sign = (i % 2 == 0) ? 1.0f : -1.0f;
        Vec3 rPos = anc.pos;
        float rRad = anc.radius * 0.78f;
        Vec3 rDir = (anc.dir * 0.55f + side * (0.72f * sign) + Vec3(0.0f, rng.nextRange(-0.10f, 0.25f), 0.0f)).normalized();

        int runSegs = 3;
        float segLen = anc.len * 0.72f;
        for (int rs = 0; rs < runSegs; ++rs) {
            rDir = (rDir + Vec3(side.x * sign * 0.14f, -0.26f * (rs + 1), side.z * sign * 0.14f)).normalized();
            Vec3 rNext = rPos + rDir * segLen;
            if (rNext.y < 0.15f) rNext.y = 0.15f;
            float rNextRad = std::max(0.004f, rRad * 0.78f);

            TreeBranchSegment rSeg;
            rSeg.startPos = rPos;
            rSeg.endPos = rNext;
            rSeg.radiusStart = rRad;
            rSeg.radiusEnd = rNextRad;
            rSeg.recursionDepth = 2;
            rSeg.branchIndex = static_cast<int>(outBranches.size());
            rSeg.windWeight = 0.90f;
            rSeg.normal = (rNext - rPos).normalized();
            outBranches.push_back(rSeg);

            if (rs >= 1) tendrilAnchors.push_back({rNext, rSeg.normal, rNextRad, segLen});
            rPos = rNext;
            rRad = rNextRad;
        }
    }

    for (size_t i = 0; i < tendrilAnchors.size(); ++i) {
        const auto& ta = tendrilAnchors[i];
        Vec3 side = cross(ta.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (side.lengthSq() < 0.01f) side = Vec3(1.0f, 0.0f, 0.0f);
        side = side.normalized();

        float sign = (i % 2 == 0) ? 1.0f : -1.0f;
        Vec3 shootDir = (ta.dir * 0.65f + side * (0.50f * sign) + Vec3(0.0f, -0.45f, 0.0f)).normalized();
        Vec3 shootEnd = ta.pos + shootDir * (ta.len * 0.62f);
        if (shootEnd.y < 0.12f) shootEnd.y = 0.12f;

        TreeBranchSegment fSeg;
        fSeg.startPos = ta.pos;
        fSeg.endPos = shootEnd;
        fSeg.radiusStart = std::min(0.009f, ta.radius * 0.85f);
        fSeg.radiusEnd = 0.003f;
        fSeg.recursionDepth = 3;
        fSeg.branchIndex = static_cast<int>(outBranches.size());
        fSeg.windWeight = 0.97f;
        fSeg.normal = (shootEnd - ta.pos).normalized();
        outBranches.push_back(fSeg);

        if (i % 2 == 0) {
            Vec3 tPos = ta.pos;
            float tRad = 0.0032f;
            Vec3 upVec(0.0f, 1.0f, 0.0f);
            for (int k = 0; k < 3; ++k) {
                float ang = static_cast<float>(k) * 2.1f + static_cast<float>(i);
                Vec3 tStep = (side * std::cos(ang) * 0.08f + upVec * std::sin(ang) * 0.07f + ta.dir * 0.06f);
                Vec3 tNext = tPos + tStep;
                TreeBranchSegment tSeg;
                tSeg.startPos = tPos;
                tSeg.endPos = tNext;
                tSeg.radiusStart = tRad;
                tSeg.radiusEnd = 0.0020f;
                tSeg.recursionDepth = 3;
                tSeg.branchIndex = static_cast<int>(outBranches.size());
                tSeg.windWeight = 0.99f;
                tSeg.normal = tStep.normalized();
                outBranches.push_back(tSeg);
                tPos = tNext;
                tRad = 0.0024f;
            }
        }
    }

    int leafCount = std::max(850, ctx.leafCount);
    TreeTypeBase::generateStojanovicLeaves(
        outBranches, outLeaves, rng,
        ctx.leafScale, ctx.leafColorR, ctx.leafColorG, ctx.leafColorB,
        ctx.treeHeight, leafCount, false, 1, ctx.terminalLeafCount, ctx.realLeafLengthMeters
    );
}

void buildDecurrentBroadleaf(
    const TreeBuildContext& ctx,
    std::vector<TreeBranchSegment>& outBranches,
    std::vector<TreeLeafData>& outLeaves,
    FastRng& rng
) {
    const bool isCacaoJorquette = (ctx.speciesId == TreeSpeciesId::CACAO_TREE);
    const bool isCitrusDome = (ctx.speciesId == TreeSpeciesId::SWEET_ORANGE ||
                               ctx.speciesId == TreeSpeciesId::LEMON_TREE);
    const bool isDogwood = (ctx.speciesId == TreeSpeciesId::FLOWERING_DOGWOOD ||
                            ctx.speciesId == TreeSpeciesId::CASHEW_TREE ||
                            isCacaoJorquette);
    const bool isAscendingPear = (ctx.speciesId == TreeSpeciesId::SWEET_PEAR ||
                                  ctx.speciesId == TreeSpeciesId::GINKGO_BILOBA);
    const bool isArchingDroop = (ctx.speciesId == TreeSpeciesId::GOLDEN_SHOWER_CASSIA ||
                                 ctx.speciesId == TreeSpeciesId::CRIMSON_BOTTLEBRUSH ||
                                 ctx.speciesId == TreeSpeciesId::BOUGAINVILLEA_STANDARD ||
                                 ctx.speciesId == TreeSpeciesId::PEACH_TREE);

    int limbCount = isCacaoJorquette ? 5 : std::clamp(4 + (ctx.branchNumber - 18) / 9, 4, 7);

    const bool isTallRainforest = (
        ctx.speciesId == TreeSpeciesId::MERU_OAK ||
        ctx.speciesId == TreeSpeciesId::SYCAMORE_FIG ||
        ctx.speciesId == TreeSpeciesId::MUGUMO_SACRED_FIG ||
        ctx.speciesId == TreeSpeciesId::NANDI_FLAME ||
        ctx.speciesId == TreeSpeciesId::MANGO_TREE ||
        ctx.speciesId == TreeSpeciesId::BREADFRUIT_TREE ||
        ctx.speciesId == TreeSpeciesId::JACKFRUIT_TREE ||
        ctx.speciesId == TreeSpeciesId::SOUTHERN_MAGNOLIA ||
        ctx.speciesId == TreeSpeciesId::CAPE_CHESTNUT ||
        ctx.speciesId == TreeSpeciesId::EUROPEAN_BEECH ||
        ctx.speciesId == TreeSpeciesId::GINKGO_BILOBA ||
        ctx.speciesId == TreeSpeciesId::SWEET_CHESTNUT ||
        ctx.speciesId == TreeSpeciesId::SAUSAGE_TREE
    );
    const bool isLowOrchard = (
        ctx.speciesId == TreeSpeciesId::HONEYCRISP_APPLE ||
        ctx.speciesId == TreeSpeciesId::SWEET_PEAR ||
        ctx.speciesId == TreeSpeciesId::PEACH_TREE ||
        ctx.speciesId == TreeSpeciesId::CASHEW_TREE ||
        isCacaoJorquette ||
        isCitrusDome ||
        ctx.speciesId == TreeSpeciesId::MEDITERRANEAN_OLIVE ||
        ctx.speciesId == TreeSpeciesId::FLOWERING_DOGWOOD ||
        ctx.speciesId == TreeSpeciesId::CRIMSON_BOTTLEBRUSH
    );

    float clearBoleFrac = isTallRainforest
        ? ((ctx.speciesId == TreeSpeciesId::MERU_OAK) ? 0.50f : 0.44f)
        : (ctx.speciesId == TreeSpeciesId::BOUGAINVILLEA_STANDARD)
            ? 0.42f
            : (ctx.isMangroveStilt ? 0.25f : (isCacaoJorquette ? 0.30f : (isCitrusDome ? 0.20f : (isLowOrchard ? 0.23f : 0.36f))));
    float leaderTopFrac = isCacaoJorquette ? 0.42f : (isCitrusDome ? 0.46f : std::min(0.72f, clearBoleFrac + 0.22f));

    int trunkSteps = std::max(10, limbCount + 4);
    float trunkH = ctx.treeHeight * leaderTopFrac;
    float stepH = trunkH / static_cast<float>(trunkSteps);
    Vec3 currPos(0.0f, 0.0f, 0.0f);
    float currR = ctx.trunkRadius;

    struct TrunkAttachmentNode {
        Vec3 pos;
        Vec3 dir;
        float radius;
        float heightFrac;
    };
    std::vector<TrunkAttachmentNode> trunkNodes;

    for (int t = 0; t < trunkSteps; ++t) {
        float nX = veg_noise3d(currPos.x * 0.35f, currPos.y * 0.35f, currPos.z * 0.35f) * (stepH * 0.10f);
        float nZ = veg_noise3d(currPos.x * 0.35f + 14.7f, currPos.y * 0.35f + 6.3f, currPos.z * 0.35f + 27.1f) * (stepH * 0.10f);
        Vec3 nextPos = currPos + Vec3(nX + rng.nextRange(-0.015f, 0.015f) * stepH, stepH, nZ + rng.nextRange(-0.015f, 0.015f) * stepH);
        float hFrac = nextPos.y / ctx.treeHeight;
        float nextR = currR * ((hFrac < clearBoleFrac) ? 0.965f : 0.89f);

        TreeBranchSegment seg;
        seg.startPos = currPos;
        seg.endPos = nextPos;
        seg.radiusStart = currR;
        seg.radiusEnd = nextR;
        seg.recursionDepth = 0;
        seg.branchIndex = static_cast<int>(outBranches.size());
        seg.windWeight = (nextPos.y / ctx.treeHeight) * 0.15f;
        seg.normal = (nextPos - currPos).normalized();
        outBranches.push_back(seg);

        if (isCacaoJorquette) {
            if (t >= trunkSteps - 2) trunkNodes.push_back({nextPos, seg.normal, nextR, hFrac});
        } else if (hFrac >= clearBoleFrac && t < trunkSteps - 1) {
            trunkNodes.push_back({nextPos, seg.normal, nextR, hFrac});
        }

        currPos = nextPos;
        currR = nextR;
    }

    if (trunkNodes.empty() && outBranches.size() >= 2) {
        const auto& mid = outBranches[outBranches.size() - 2];
        trunkNodes.push_back({mid.endPos, mid.normal, mid.radiusEnd, mid.endPos.y / ctx.treeHeight});
    }

    float baseAz = rng.nextFloat01() * 6.28318f;
    size_t numNodes = trunkNodes.size();
    const int maxAllowedBranches = std::clamp(ctx.branchNumber * 8, 220, 480);

    struct BroadleafBranchTip {
        Vec3 pos;
        Vec3 dir;
        float radius;
        float length;
    };
    std::vector<BroadleafBranchTip> order2Tips;
    std::vector<BroadleafBranchTip> order3Tips;

    for (int l = 0; l < limbCount; ++l) {
        size_t nodeIdx = (numNodes > 0)
            ? std::min(numNodes - 1, static_cast<size_t>((static_cast<float>(l) / static_cast<float>(limbCount)) * numNodes))
            : 0;
        const auto& node = (numNodes > 0) ? trunkNodes[nodeIdx] : TrunkAttachmentNode{currPos, Vec3(0, 1, 0), currR, 0.5f};

        float az = isCacaoJorquette
            ? (baseAz + static_cast<float>(l) * (6.28318f / static_cast<float>(limbCount)) + rng.nextRange(-0.10f, 0.10f))
            : (baseAz + static_cast<float>(l) * TreeTypeBase::GOLDEN_ANGLE_RAD + rng.nextRange(-0.14f, 0.14f));
        float pitchDeg = isCacaoJorquette
            ? rng.nextRange(58.0f, 68.0f)
            : (isCitrusDome
                ? rng.nextRange(54.0f, 66.0f)
                : (isDogwood ? std::max(54.0f, ctx.branchAngleDeg)
                             : (isAscendingPear ? std::min(35.0f, ctx.branchAngleDeg) : ctx.branchAngleDeg))) + rng.nextRange(-4.5f, 4.5f);
        float pitchRad = pitchDeg * (3.14159f / 180.0f);

        Vec3 dir(
            std::sin(pitchRad) * std::cos(az),
            isDogwood ? (std::cos(pitchRad) * 0.42f + 0.16f)
                      : (isCitrusDome ? (std::cos(pitchRad) * 0.56f + 0.14f)
                                      : (isAscendingPear ? (std::cos(pitchRad) * 0.80f + 0.40f) : (std::cos(pitchRad) * 0.65f + 0.35f))),
            std::sin(pitchRad) * std::sin(az)
        );
        dir = dir.normalized();

        float heightFrac = static_cast<float>(l) / static_cast<float>(std::max(1, limbCount));
        float baseReach = (isCitrusDome || isLowOrchard) ? 0.38f : 0.34f;
        float limbLen = (ctx.treeHeight * (baseReach - heightFrac * 0.09f)) * rng.nextRange(0.88f, 1.12f);

        int limbSteps = 3;
        Vec3 lStart = node.pos;
        float lStartR = std::min(0.28f, node.radius * 0.56f);
        Vec3 proxLimbPos = node.pos;
        Vec3 proxLimbDir = dir;
        float proxLimbR = lStartR;
        Vec3 midLimbPos = node.pos;
        Vec3 midLimbDir = dir;
        float midLimbR = lStartR;

        for (int s = 0; s < limbSteps; ++s) {
            float t1 = static_cast<float>(s + 1) / static_cast<float>(limbSteps);
            Vec3 straightEnd = node.pos + dir * (limbLen * t1);

            float gnarlAmp = (s < limbSteps - 1) ? (limbLen * 0.055f) : 0.0f;
            float nX = veg_noise3d(straightEnd.x * 0.4f, straightEnd.y * 0.4f, straightEnd.z * 0.4f) * gnarlAmp;
            float nY = veg_noise3d(straightEnd.x * 0.4f + 11.1f, straightEnd.y * 0.4f + 8.9f, straightEnd.z * 0.4f) * (gnarlAmp * 0.35f);
            float nZ = veg_noise3d(straightEnd.x * 0.4f + 23.5f, straightEnd.y * 0.4f + 17.3f, straightEnd.z * 0.4f + 13.7f) * gnarlAmp;

            Vec3 lEnd = straightEnd + Vec3(nX, nY, nZ);
            float lEndR = lStartR * (1.0f - (t1 * 0.48f) / 3.0f);
            if (s == limbSteps - 1) lEndR = std::max(0.022f, lStartR * 0.62f);

            TreeBranchSegment lSeg;
            lSeg.startPos = lStart;
            lSeg.endPos = lEnd;
            lSeg.radiusStart = lStartR;
            lSeg.radiusEnd = lEndR;
            lSeg.recursionDepth = 1;
            lSeg.branchIndex = static_cast<int>(outBranches.size());
            lSeg.windWeight = std::min(1.0f, (lEnd.y / ctx.treeHeight) * 0.70f);
            lSeg.normal = (lEnd - lStart).normalized();
            outBranches.push_back(lSeg);

            if (s == 0) {
                proxLimbPos = lEnd;
                proxLimbDir = lSeg.normal;
                proxLimbR = lEndR;
            } else if (s == 1) {
                midLimbPos = lEnd;
                midLimbDir = lSeg.normal;
                midLimbR = lEndR;
            }
            lStart = lEnd;
            lStartR = lEndR;
        }

        Vec3 tip = lStart;
        Vec3 side = cross(dir, Vec3(0.0f, 1.0f, 0.0f));
        if (side.lengthSq() < 0.01f) side = cross(dir, Vec3(1.0f, 0.0f, 0.0f));
        side = side.normalized();
        Vec3 up = cross(dir, side).normalized();

        // Staggered proximal (s=0) AND mid (s=1) Order 2 lateral sub-branches along every primary scaffold!
        struct LatSpawn { Vec3 pos; Vec3 dir; float r; float sign; float scale; };
        const LatSpawn laterals[2] = {
            { proxLimbPos, proxLimbDir, proxLimbR, (l % 2 == 0 ? -1.0f : 1.0f), 0.36f },
            { midLimbPos,  midLimbDir,  midLimbR,  (l % 2 == 0 ? 1.0f : -1.0f), 0.44f }
        };
        for (int li = 0; li < 2; ++li) {
            const auto& sp = laterals[li];
            float yBias = isDogwood ? 0.04f : (isArchingDroop ? -0.06f : (isCitrusDome ? 0.08f : 0.14f));
            Vec3 latDir = (sp.dir * 0.70f + side * (0.64f * sp.sign) + Vec3(0.0f, yBias, 0.0f)).normalized();
            float latLen = limbLen * sp.scale * rng.nextRange(0.88f, 1.12f);
            Vec3 latPos = sp.pos;
            float latR = sp.r * 0.64f;

            for (int ss = 0; ss < 2; ++ss) {
                float subLen = latLen * 0.50f;
                Vec3 latEnd = latPos + latDir * subLen + Vec3(0.0f, ss * 0.06f * subLen, 0.0f);
                float latEndR = (ss == 1) ? std::max(0.012f, latR * 0.65f) : (latR * 0.78f);

                TreeBranchSegment latSeg;
                latSeg.startPos = latPos;
                latSeg.endPos = latEnd;
                latSeg.radiusStart = latR;
                latSeg.radiusEnd = latEndR;
                latSeg.recursionDepth = 2;
                latSeg.branchIndex = static_cast<int>(outBranches.size());
                latSeg.windWeight = std::min(1.0f, (latEnd.y / ctx.treeHeight) * 0.82f);
                latSeg.normal = (latEnd - latPos).normalized();
                outBranches.push_back(latSeg);

                if (ss == 0) {
                    order2Tips.push_back({latEnd, latDir, latEndR, latLen * 0.80f});
                }
                latPos = latEnd;
                latR = latEndR;
            }
            order2Tips.push_back({latPos, latDir, latR, latLen});
        }

        // Distal Order 2 forks (2-3 radiating sub-boughs from scaffold tip)
        int distalForks = (isCitrusDome || isLowOrchard || ctx.branchNumber >= 36) ? 3 : 2;
        for (int tw = 0; tw < distalForks; ++tw) {
            float rotPhi = static_cast<float>(tw) * (6.28318f / static_cast<float>(distalForks)) + (l * 0.7f) + rng.nextRange(-0.20f, 0.20f);
            float angleDev = 0.46f + rng.nextRange(-0.06f, 0.06f);

            Vec3 twDir = (dir * std::cos(angleDev) + (side * std::cos(rotPhi) + up * std::sin(rotPhi)) * std::sin(angleDev)).normalized();
            twDir.y = isDogwood
                ? std::clamp(twDir.y * 0.45f, -0.05f, 0.20f)
                : (isArchingDroop ? (twDir.y - 0.10f) : std::max(0.08f, twDir.y + (isCitrusDome ? 0.06f : 0.14f)));
            twDir = twDir.normalized();

            float twLen = limbLen * 0.46f * rng.nextRange(0.88f, 1.12f);
            Vec3 twPos = tip;
            float twR = lStartR * 0.70f;

            for (int ss = 0; ss < 2; ++ss) {
                float subLen = twLen * 0.50f;
                Vec3 twEnd = twPos + twDir * subLen;
                float twEndR = (ss == 1) ? std::max(0.012f, twR * 0.65f) : (twR * 0.78f);

                TreeBranchSegment twSeg;
                twSeg.startPos = twPos;
                twSeg.endPos = twEnd;
                twSeg.radiusStart = twR;
                twSeg.radiusEnd = twEndR;
                twSeg.recursionDepth = 2;
                twSeg.branchIndex = static_cast<int>(outBranches.size());
                twSeg.windWeight = std::min(1.0f, (twEnd.y / ctx.treeHeight) * 0.86f);
                twSeg.normal = twDir;
                outBranches.push_back(twSeg);

                if (ss == 0) {
                    order2Tips.push_back({twEnd, twDir, twEndR, twLen * 0.78f});
                }
                twPos = twEnd;
                twR = twEndR;
            }
            order2Tips.push_back({twPos, twDir, twR, twLen});
        }
    }

    // Apical crown terminal boughs emerging from trunk apex to fill upper central canopy dome
    if (!isCacaoJorquette) {
        Vec3 apexPos = currPos;
        float apexR = currR;
        int apicalCount = (limbCount >= 5 || isCitrusDome) ? 4 : 3;
        for (int a = 0; a < apicalCount; ++a) {
            float az = baseAz + static_cast<float>(a) * (6.28318f / static_cast<float>(apicalCount)) + rng.nextRange(-0.14f, 0.14f);
            float pitchRad = ((isCitrusDome ? 34.0f : 28.0f) + rng.nextRange(-4.0f, 4.0f)) * (3.14159f / 180.0f);
            Vec3 aDir = Vec3(std::sin(pitchRad) * std::cos(az), std::cos(pitchRad), std::sin(pitchRad) * std::sin(az)).normalized();

            float aLen = (ctx.treeHeight * (isCitrusDome ? 0.26f : 0.22f)) * rng.nextRange(0.86f, 1.14f);
            Vec3 aEnd = apexPos + aDir * aLen;
            float aStartR = std::min(0.24f, apexR * 0.74f);
            float aEndR = std::max(0.014f, aStartR * 0.65f);

            TreeBranchSegment aSeg;
            aSeg.startPos = apexPos;
            aSeg.endPos = aEnd;
            aSeg.radiusStart = aStartR;
            aSeg.radiusEnd = aEndR;
            aSeg.recursionDepth = 1;
            aSeg.branchIndex = static_cast<int>(outBranches.size());
            aSeg.windWeight = 0.85f;
            aSeg.normal = aDir;
            outBranches.push_back(aSeg);

            for (int tw = 0; tw < 2; ++tw) {
                float twDev = (tw == 0 ? 0.38f : -0.38f) + rng.nextRange(-0.06f, 0.06f);
                Vec3 twSide = cross(aDir, Vec3(0.0f, 1.0f, 0.0f));
                if (twSide.lengthSq() < 0.01f) twSide = cross(aDir, Vec3(1.0f, 0.0f, 0.0f));
                twSide = twSide.normalized();
                Vec3 twDir = (aDir * std::cos(twDev) + twSide * std::sin(twDev)).normalized();
                twDir.y = std::max(0.18f, twDir.y + 0.12f);
                twDir = twDir.normalized();

                float twLen = aLen * 0.54f;
                Vec3 twTip = aEnd + twDir * twLen;
                float twEndR = std::max(0.010f, aEndR * 0.65f);

                TreeBranchSegment twSeg;
                twSeg.startPos = aEnd;
                twSeg.endPos = twTip;
                twSeg.radiusStart = aEndR;
                twSeg.radiusEnd = twEndR;
                twSeg.recursionDepth = 2;
                twSeg.branchIndex = static_cast<int>(outBranches.size());
                twSeg.windWeight = 0.92f;
                twSeg.normal = twDir;
                outBranches.push_back(twSeg);

                order2Tips.push_back({twTip, twDir, twEndR, twLen});
            }
        }
    }

    // Order 3 secondary branchlets
    for (size_t i = 0; i < order2Tips.size(); ++i) {
        if (static_cast<int>(outBranches.size()) >= maxAllowedBranches) break;
        const auto& o2 = order2Tips[i];
        Vec3 tSide = cross(o2.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (tSide.lengthSq() < 0.01f) tSide = cross(o2.dir, Vec3(1.0f, 0.0f, 0.0f));
        tSide = tSide.normalized();
        Vec3 tUp = cross(o2.dir, tSide).normalized();

        for (int f = 0; f < 2; ++f) {
            if (static_cast<int>(outBranches.size()) >= maxAllowedBranches) break;
            float phi = (f == 0 ? 0.44f : -0.44f) + rng.nextRange(-0.08f, 0.08f);
            float roll = (f == 0 ? 0.24f : -0.24f);
            Vec3 fDir = (o2.dir * 0.84f + (tSide * std::sin(phi) + tUp * std::sin(roll))).normalized();
            fDir.y = isArchingDroop
                ? (fDir.y - 0.18f)
                : (isDogwood ? std::clamp(fDir.y * 0.40f, -0.05f, 0.18f)
                             : std::max(isCitrusDome ? -0.04f : 0.08f, fDir.y + 0.08f));
            fDir = fDir.normalized();

            float fLen = o2.length * 0.50f * rng.nextRange(0.85f, 1.15f);
            Vec3 fTip = o2.pos + fDir * fLen;
            float fEndR = std::max(0.006f, o2.radius * 0.58f);

            TreeBranchSegment fSeg;
            fSeg.startPos = o2.pos;
            fSeg.endPos = fTip;
            fSeg.radiusStart = std::min(0.022f, o2.radius * 0.78f);
            fSeg.radiusEnd = fEndR;
            fSeg.recursionDepth = 3;
            fSeg.branchIndex = static_cast<int>(outBranches.size());
            fSeg.windWeight = 0.94f;
            fSeg.normal = fDir;
            outBranches.push_back(fSeg);

            order3Tips.push_back({fTip, fDir, fEndR, fLen});
        }
    }

    // Order 4 outer leafy & fruiting fine shoots
    for (size_t i = 0; i < order3Tips.size(); ++i) {
        if (static_cast<int>(outBranches.size()) >= maxAllowedBranches) break;
        const auto& o3 = order3Tips[i];
        Vec3 tSide = cross(o3.dir, Vec3(0.0f, 1.0f, 0.0f));
        if (tSide.lengthSq() < 0.01f) tSide = cross(o3.dir, Vec3(1.0f, 0.0f, 0.0f));
        tSide = tSide.normalized();
        Vec3 tUp = cross(o3.dir, tSide).normalized();

        for (int f = 0; f < 2; ++f) {
            if (static_cast<int>(outBranches.size()) >= maxAllowedBranches) break;
            float phi = (f == 0 ? 0.36f : -0.36f) + rng.nextRange(-0.10f, 0.10f);
            float roll = ((i + f) % 2 == 0 ? 0.20f : -0.20f);
            Vec3 fDir = (o3.dir * 0.86f + (tSide * std::sin(phi) + tUp * std::sin(roll))).normalized();
            fDir.y = isArchingDroop ? (fDir.y - 0.14f) : std::max(-0.08f, fDir.y + 0.04f);
            fDir = fDir.normalized();

            float fLen = std::max(0.14f, o3.length * 0.56f * rng.nextRange(0.85f, 1.15f));
            Vec3 fTip = o3.pos + fDir * fLen;

            TreeBranchSegment sSeg;
            sSeg.startPos = o3.pos;
            sSeg.endPos = fTip;
            sSeg.radiusStart = std::min(0.010f, o3.radius * 0.80f);
            sSeg.radiusEnd = 0.004f;
            sSeg.recursionDepth = 4;
            sSeg.branchIndex = static_cast<int>(outBranches.size());
            sSeg.windWeight = 0.98f;
            sSeg.normal = fDir;
            outBranches.push_back(sSeg);
        }
    }

    int leafCount = std::max(limbCount * 160, ctx.leafCount);
    TreeTypeBase::generateStojanovicLeaves(
        outBranches, outLeaves, rng,
        ctx.leafScale, ctx.leafColorR, ctx.leafColorG, ctx.leafColorB,
        ctx.treeHeight, leafCount, false, 2, ctx.terminalLeafCount, ctx.realLeafLengthMeters
    );
}

} // namespace categories
} // namespace vegetation
